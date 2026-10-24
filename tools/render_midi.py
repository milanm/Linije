"""Render a General MIDI file through an SF2 soundfont to a stereo WAV.

Used to turn the original Zvuk.mid into the web version's music. A small
SoundFont 2 synth in numpy: preset/instrument zone lookup, sample pitch
shifting with loops, DAHDSR volume envelope, initial low-pass filter, pan,
channel volume/expression automation, pitch bend, sustain pedal, exclusive
classes and a simple reverb send. No LFOs or modulation envelope.

    python tools/render_midi.py GeneralUser-GS.sf2 Zvuk.mid zvuk.wav
    ffmpeg -i zvuk.wav -codec:a libmp3lame -b:a 128k web/assets/zvuk.mp3

Needs numpy and scipy. GeneralUser GS: https://github.com/mrbumpy409/GeneralUser-GS
"""

import struct
import sys
import wave

import numpy as np
from scipy.signal import fftconvolve, lfilter

SR = 44100
MAX_RELEASE = 3.0


# ---------------------------------------------------------------- SF2 ----

def chunks(data, off, end):
    while off < end:
        cid = data[off:off + 4]
        size = struct.unpack_from('<I', data, off + 4)[0]
        yield cid, off + 8, size
        off += 8 + size + (size & 1)


class SoundFont:
    def __init__(self, path):
        data = open(path, 'rb').read()
        assert data[:4] == b'RIFF' and data[8:12] == b'sfbk'
        pd = {}
        for cid, off, size in chunks(data, 12, len(data)):
            if cid != b'LIST':
                continue
            kind = data[off:off + 4]
            for sid, soff, ssize in chunks(data, off + 4, off + size):
                if kind == b'sdta' and sid == b'smpl':
                    self.smpl = np.frombuffer(data, '<i2', ssize // 2, soff).astype(np.float32) / 32768.0
                elif kind == b'pdta':
                    pd[sid] = data[soff:soff + ssize]

        def recs(name, fmt):
            size = struct.calcsize(fmt)
            raw = pd[name]
            return [struct.unpack_from(fmt, raw, i) for i in range(0, len(raw), size)]

        self.phdr = recs(b'phdr', '<20sHHHIII')
        self.pbag = recs(b'pbag', '<HH')
        self.pgen = recs(b'pgen', '<HH')
        self.inst = recs(b'inst', '<20sH')
        self.ibag = recs(b'ibag', '<HH')
        self.igen = recs(b'igen', '<HH')
        self.shdr = recs(b'shdr', '<20sIIIIIBbHH')
        self.presets = {(p[2], p[1]): i for i, p in enumerate(self.phdr[:-1])}
        self._inst_cache = {}
        self._preset_cache = {}

    @staticmethod
    def _zone_gens(bags, gens, i):
        out = {}
        for oper, amount in gens[bags[i][0]:bags[i + 1][0]]:
            if oper in (43, 44):
                out[oper] = (amount & 0xFF, amount >> 8)
            else:
                out[oper] = amount - 65536 if amount >= 32768 and oper not in (41, 53) else amount
        return out

    def _zones(self, first, last, bags, gens, terminal):
        zones, glob = [], {}
        for i in range(first, last):
            g = self._zone_gens(bags, gens, i)
            if terminal not in g:
                if i == first:
                    glob = g
                continue
            zones.append({**glob, **g})
        return zones

    def instrument_zones(self, idx):
        if idx not in self._inst_cache:
            self._inst_cache[idx] = self._zones(self.inst[idx][1], self.inst[idx + 1][1], self.ibag, self.igen, 53)
        return self._inst_cache[idx]

    def preset_zones(self, bank, prog):
        key = (bank, prog)
        if key not in self._preset_cache:
            idx = self.presets.get(key)
            if idx is None:
                idx = self.presets.get((128, 0) if bank == 128 else (0, prog))
            if idx is None:
                self._preset_cache[key] = []
            else:
                self._preset_cache[key] = self._zones(self.phdr[idx][3], self.phdr[idx + 1][3], self.pbag, self.pgen, 41)
        return self._preset_cache[key]

    def voices(self, bank, prog, key, vel):
        """Generator sets for every sample a note triggers."""
        out = []
        for pz in self.preset_zones(bank, prog):
            klo, khi = pz.get(43, (0, 127))
            vlo, vhi = pz.get(44, (0, 127))
            if not (klo <= key <= khi and vlo <= vel <= vhi):
                continue
            for iz in self.instrument_zones(pz[41]):
                klo, khi = iz.get(43, (0, 127))
                vlo, vhi = iz.get(44, (0, 127))
                if not (klo <= key <= khi and vlo <= vel <= vhi):
                    continue
                g = dict(iz)
                for oper, val in pz.items():
                    if oper in (41, 43, 44, 0, 1, 2, 3, 4, 12, 45, 50, 53, 54, 57, 58, 46, 47):
                        continue
                    g[oper] = g.get(oper, DEFAULTS.get(oper, 0)) + val
                out.append(g)
        return out


DEFAULTS = {8: 13500, 9: 0, 17: 0, 33: -12000, 34: -12000, 35: -12000, 36: -12000, 37: 0,
            38: -12000, 39: 0, 40: 0, 48: 0, 51: 0, 52: 0, 54: 0, 56: 100, 57: 0, 58: -1, 16: 0}


def gen(g, oper):
    return g.get(oper, DEFAULTS.get(oper, 0))


def tc2s(tc):
    return 0.0 if tc <= -12000 else 2.0 ** (tc / 1200.0)


# ---------------------------------------------------------------- MIDI ----

def read_midi(path):
    d = open(path, 'rb').read()
    _, ntrk, div = struct.unpack('>HHH', d[8:14])
    pos, events = 14, []

    def vlq(p):
        v = 0
        while True:
            c = d[p]
            p += 1
            v = (v << 7) | (c & 0x7F)
            if not c & 0x80:
                return v, p

    for t in range(ntrk):
        ln = struct.unpack('>I', d[pos + 4:pos + 8])[0]
        p, end, tick, status, seq = pos + 8, pos + 8 + ln, 0, 0, 0
        while p < end:
            dt, p = vlq(p)
            tick += dt
            b = d[p]
            if b == 0xFF:
                typ = d[p + 1]
                ln2, p2 = vlq(p + 2)
                if typ == 0x51:
                    events.append((tick, t, seq, 'tempo', int.from_bytes(d[p2:p2 + ln2], 'big')))
                p = p2 + ln2
            elif b in (0xF0, 0xF7):
                ln2, p2 = vlq(p + 1)
                p = p2 + ln2
            else:
                if b & 0x80:
                    status = b
                    p += 1
                kind, ch = status & 0xF0, status & 0x0F
                if kind in (0xC0, 0xD0):
                    a, c = d[p], 0
                    p += 1
                else:
                    a, c = d[p], d[p + 1]
                    p += 2
                events.append((tick, t, seq, kind, (ch, a, c)))
            seq += 1
        pos = end
    events.sort(key=lambda e: (e[0], e[1], e[2]))
    # Ticks to seconds with the tempo map.
    out, us, last_tick, sec = [], 500000, 0, 0.0
    for tick, _, _, kind, val in events:
        sec += (tick - last_tick) * us / div / 1e6
        last_tick = tick
        if kind == 'tempo':
            us = val
        else:
            out.append((sec, kind, val))
    return out


# --------------------------------------------------------------- render ----

class Channel:
    def __init__(self, n):
        self.prog, self.bank = 0, 128 if n == 9 else 0
        self.vol, self.expr, self.pan, self.rev = 100, 127, 64, 40
        self.sustain = False
        self.vol_ev = [(-1.0, 100)]   # (time, value) timelines, read per sample
        self.expr_ev = [(-1.0, 127)]
        self.bend = []  # (time, semitones)
        self.bend_range = 2.0
        self.rpn = [127, 127]


def biquad_lowpass(fc, q):
    w0 = 2 * np.pi * min(fc, SR * 0.45) / SR
    alpha = np.sin(w0) / (2 * q)
    cosw = np.cos(w0)
    b = np.array([(1 - cosw) / 2, 1 - cosw, (1 - cosw) / 2])
    a = np.array([1 + alpha, -2 * cosw, 1 - alpha])
    return b / a[0], a / a[0]


def step_at(events, times):
    ts = np.array([e[0] for e in events])
    vs = np.array([e[1] for e in events], dtype=np.float64)
    return vs[np.clip(np.searchsorted(ts, times, side='right') - 1, 0, None)]


def bend_at(ch, times):
    """Pitch bend in semitones for each output time."""
    if not ch.bend:
        return np.zeros_like(times)
    ts = np.array([b[0] for b in ch.bend])
    vs = np.array([b[1] for b in ch.bend])
    idx = np.searchsorted(ts, times, side='right') - 1
    return np.where(idx >= 0, vs[np.clip(idx, 0, None)], 0.0)


def render_voice(sf, g, key, vel, ch, pan_cc, rev_cc, t_on, t_off, out_l, out_r, wet):
    sh = sf.shdr[g[53]]
    _, s_start, s_end, s_ls, s_le, s_rate, s_pitch, s_corr, _, _ = sh
    start = s_start + gen(g, 0) + 32768 * gen(g, 4)
    end = s_end + gen(g, 1) + 32768 * gen(g, 12)
    ls = s_ls + gen(g, 2) + 32768 * gen(g, 45)
    le = s_le + gen(g, 3) + 32768 * gen(g, 50)
    mode = gen(g, 54) & 3
    looping = mode in (1, 3) and le > ls + 1
    root = gen(g, 58) if gen(g, 58) >= 0 else s_pitch
    cents = (key - root) * gen(g, 56) + gen(g, 51) * 100 + gen(g, 52) + s_corr
    base = 2.0 ** (cents / 1200.0) * s_rate / SR

    # Volume envelope.
    delay = tc2s(gen(g, 33))
    attack = tc2s(gen(g, 34))
    hold = tc2s(gen(g, 35) + (60 - key) * gen(g, 39))
    decay = tc2s(gen(g, 36) + (60 - key) * gen(g, 40))
    sus_db = min(max(gen(g, 37), 0), 1440) / 10.0
    release = min(tc2s(gen(g, 38)), MAX_RELEASE)

    held = max(t_off - t_on, 0.005)
    total = held + release + 0.01
    n = int(total * SR)
    if n <= 0:
        return
    t = np.arange(n) / SR

    # dB attenuation over time, before release.
    db = np.full(n, 0.0)
    a_end = delay + attack
    h_end = a_end + hold
    in_decay = t >= h_end
    if decay > 0:
        db = np.where(in_decay, np.minimum((t - h_end) / decay * 100.0, sus_db), db)
    else:
        db = np.where(in_decay, sus_db, db)
    amp = 10 ** (-db / 20.0)
    if attack > 0:
        amp = np.where(t < a_end, np.clip((t - delay) / attack, 0, 1), amp)
    amp = np.where(t < delay, 0.0, amp)
    # Release: falls 100 dB over the release time from wherever it was.
    rel = t >= held
    if rel.any():
        i0 = int(np.argmax(rel))
        level = amp[i0 - 1] if i0 > 0 else amp[0]
        rel_db = (t[rel] - held) / max(release, 0.005) * 100.0
        amp[rel] = level * 10 ** (-rel_db / 20.0)
    keep = amp > 1e-4
    if not keep.any():
        return
    n = int(np.nonzero(keep)[0][-1]) + 1
    t, amp = t[:n], amp[:n]

    # Sample positions with pitch bend.
    inc = base * 2.0 ** (bend_at(ch, t_on + t) / 12.0)
    pos = np.concatenate(([0.0], np.cumsum(inc[:-1]))) + start
    if looping:
        over = pos >= le
        pos[over] = ls + np.mod(pos[over] - ls, le - ls)
    else:
        valid = pos < end - 1
        if not valid.any():
            return
        n = int(np.nonzero(valid)[0][-1]) + 1
        pos, amp = pos[:n], amp[:n]
    i = pos.astype(np.int64)
    frac = (pos - i).astype(np.float32)
    i = np.clip(i, 0, len(sf.smpl) - 2)
    sig = sf.smpl[i] * (1 - frac) + sf.smpl[i + 1] * frac

    fc_cents = gen(g, 8)
    if fc_cents < 13000:
        fc = 8.176 * 2.0 ** (fc_cents / 1200.0)
        q = max(10 ** (gen(g, 9) / 200.0) * 0.707, 0.5)
        b, a = biquad_lowpass(fc, q)
        sig = lfilter(b, a, sig).astype(np.float32)

    atten_cb = max(gen(g, 48), 0) * 0.4  # EMU attenuation scale, as FluidSynth uses
    tt = t_on + t[:len(sig)]
    gain = 10 ** (-atten_cb / 200.0) * (vel / 127.0) ** 2         * (step_at(ch.vol_ev, tt) / 127.0) ** 2 * (step_at(ch.expr_ev, tt) / 127.0) ** 2
    pan = np.clip((pan_cc - 64) / 63.0 + gen(g, 17) / 500.0, -1, 1)
    gl = np.cos((pan + 1) * np.pi / 4)
    gr = np.sin((pan + 1) * np.pi / 4)
    sig = (sig * amp[:len(sig)] * gain).astype(np.float32)
    s0 = int(t_on * SR)
    s1 = min(s0 + len(sig), len(out_l))
    sig = sig[:s1 - s0]
    out_l[s0:s1] += sig * gl
    out_r[s0:s1] += sig * gr
    send = (rev_cc / 127.0) * (1 + gen(g, 16) / 1000.0)
    wet[s0:s1] += sig * send


def render(sf, events):
    length = events[-1][0] + MAX_RELEASE + 1
    n = int(length * SR)
    out_l = np.zeros(n, np.float32)
    out_r = np.zeros(n, np.float32)
    wet = np.zeros(n, np.float32)
    chans = [Channel(i) for i in range(16)]
    active = {}      # (ch, key) -> list of notes
    pending = []     # finished notes to render: (ch, key, vel, t_on, t_off, gens, excl)
    sustained = {i: [] for i in range(16)}

    def note_off(c, key, t):
        for note in active.pop((c, key), []):
            if chans[c].sustain:
                sustained[c].append((note, t))
            else:
                pending.append((*note, t))

    for t, kind, (c, a, b) in events:
        ch = chans[c]
        if kind == 0x90 and b > 0:
            note_off(c, a, t)
            gens = sf.voices(ch.bank, ch.prog, a, b)
            excl = {gen(g, 57) for g in gens} - {0}
            if excl:
                for (cc, kk), notes in list(active.items()):
                    if cc == c and any(excl & n[5] for n in notes):
                        for note in active.pop((cc, kk)):
                            pending.append((*note, t))
            active.setdefault((c, a), []).append((c, a, b, t, gens, excl, ch.pan, ch.rev))
        elif kind == 0x80 or (kind == 0x90 and b == 0):
            note_off(c, a, t)
        elif kind == 0xB0:
            if a == 0:
                ch.bank = 128 if c == 9 else b
            elif a == 7:
                ch.vol_ev.append((t, b))
            elif a == 10:
                ch.pan = b
            elif a == 11:
                ch.expr_ev.append((t, b))
            elif a == 91:
                ch.rev = b
            elif a == 64:
                ch.sustain = b >= 64
                if not ch.sustain:
                    for note, _ in sustained[c]:
                        pending.append((*note, t))
                    sustained[c] = []
            elif a in (101, 100):
                ch.rpn[0 if a == 101 else 1] = b
            elif a == 6 and ch.rpn == [0, 0]:
                ch.bend_range = float(b)
            elif a in (120, 123):
                for (cc, kk) in [k for k in active if k[0] == c]:
                    note_off(cc, kk, t)
        elif kind == 0xC0:
            ch.prog = a
        elif kind == 0xE0:
            ch.bend.append((t, ((b << 7 | a) - 8192) / 8192.0 * ch.bend_range))

    end_t = events[-1][0]
    for notes in active.values():
        for note in notes:
            pending.append((*note, end_t))
    for c in range(16):
        for note, _ in sustained[c]:
            pending.append((*note, end_t))

    print(f'{len(pending)} notes', file=sys.stderr)
    for k, (c, key, vel, t_on, gens, excl, pan_cc, rev_cc, t_off) in enumerate(pending):
        for g in gens:
            render_voice(sf, g, key, vel, chans[c], pan_cc, rev_cc, t_on, t_off, out_l, out_r, wet)
        if k % 500 == 0:
            print(f'  {k}/{len(pending)}', file=sys.stderr)

    # Reverb: decaying stereo noise impulse response.
    rng = np.random.default_rng(1)
    ir_len = int(1.8 * SR)
    decay = np.exp(-np.arange(ir_len) / (0.45 * SR)).astype(np.float32)
    ir_l = rng.standard_normal(ir_len).astype(np.float32) * decay
    ir_r = rng.standard_normal(ir_len).astype(np.float32) * decay
    ir_l[:int(0.02 * SR)] = 0
    ir_r[:int(0.025 * SR)] = 0
    scale = 0.18 / np.sqrt((ir_l ** 2).sum())
    out_l += fftconvolve(wet, ir_l * scale)[:n].astype(np.float32)
    out_r += fftconvolve(wet, ir_r * scale)[:n].astype(np.float32)

    mix = np.stack([out_l, out_r], axis=1)
    last = np.nonzero(np.abs(mix).max(axis=1) > 1e-4)[0]
    mix = mix[:last[-1] + 1] if len(last) else mix
    peak = np.abs(mix).max()
    mix *= 0.89 / peak
    return mix


def main():
    sf2, mid, out = sys.argv[1:4]
    sf = SoundFont(sf2)
    events = read_midi(mid)
    mix = render(sf, events)
    pcm = (np.clip(mix, -1, 1) * 32767).astype('<i2')
    with wave.open(out, 'wb') as w:
        w.setnchannels(2)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(pcm.tobytes())
    print(f'{len(mix) / SR:.1f}s written to {out}', file=sys.stderr)


if __name__ == '__main__':
    main()
