"""Convert the original Windows resources into assets for the web version.

Reads res/*.bmp|gif|cur|ico, the *.wav sounds and Linije.chm from the repo
root and writes web/assets/ and web/help/. The output is committed, so this
only needs to run again when the original resources change.

    python tools/build_web_assets.py

Needs Pillow. The help pages are extracted with hh.exe, so that step only runs
on Windows.
"""

import json
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
RES = ROOT / "res"
WEB = ROOT / "web"
ASSETS = WEB / "assets"

SHAPES = ["kugla", "cube", "romb", "trougao", "zvezda"]
VARIANTS = ["", "over", "anim", "boom"]

BITMAPS = {
    "tabla": "tabla.bmp",
    "meni": "meni.bmp",
    "sledece": "sledece.bmp",
    "sledece-klik": "sledeceK.bmp",
    "sledece-neaktivno": "sledeceU.bmp",
    "izlaz": "izlaz.bmp",
    "izlaz-klik": "izlaz_klik.bmp",
    "x-malo": "x_malo.bmp",
    "x-malo-klik": "x_malo_klik.bmp",
    "x": "x.bmp",
    "x-preko": "xpreko.bmp",
    "x-klik": "xklik.bmp",
    "min-neaktivno": "minneaktivno.bmp",
    "sistemski": "levodugme.bmp",
    "rezultati": "rezultati.bmp",
    "unesi": "unesi.bmp",
    "oigri": "Oigri.bmp",
    "bonus": "rezultat.bmp",
}

SOUNDS = ["AddBall.wav", "bonus.wav", "DelBall.wav", "GameOver.wav", "loptica.wav"]


def gif_frames(path):
    """Return (frames, durations_ms) with every frame composited to RGBA."""
    im = Image.open(path)
    frames, durations = [], []
    for i in range(getattr(im, "n_frames", 1)):
        im.seek(i)
        frames.append(im.convert("RGBA"))
        durations.append(im.info.get("duration") or 30)
    return frames, durations


def save_strip(frames, out):
    w, h = frames[0].size
    strip = Image.new("RGBA", (w * len(frames), h), (0, 0, 0, 0))
    for i, f in enumerate(frames):
        strip.paste(f, (i * w, 0))
    strip.save(out, optimize=True)


def build_sprites():
    meta = {}
    for shape in SHAPES + ["zidic"]:
        for variant in VARIANTS:
            src = RES / f"{shape}{variant}.gif"
            if not src.exists():
                continue
            frames, durations = gif_frames(src)
            name = f"{shape}-{variant}" if variant else shape
            save_strip(frames, ASSETS / f"{name}.png")
            w, h = frames[0].size
            meta[name] = {"w": w, "h": h, "frames": len(frames), "durations": durations}
    (ASSETS / "sprites.json").write_text(json.dumps(meta, indent=1), encoding="utf-8")


def cursor_png(path):
    """Decode a 4-bit .cur by hand: Pillow ignores its AND mask, so the
    transparent area comes out black."""
    d = path.read_bytes()
    w, h = d[6], d[7]
    off = int.from_bytes(d[18:22], "little")
    bpp = int.from_bytes(d[off + 14:off + 16], "little")
    if bpp != 4:
        raise ValueError(f"{path.name}: expected a 4-bit cursor, got {bpp}-bit")
    pal_off = off + int.from_bytes(d[off:off + 4], "little")
    palette = [tuple(d[pal_off + 4 * i + 2::-1][:3]) for i in range(16)]
    xor_off = pal_off + 16 * 4
    xor_stride = ((w * 4 + 31) // 32) * 4
    and_off = xor_off + xor_stride * h
    and_stride = ((w + 31) // 32) * 4
    img = Image.new("RGBA", (w, h))
    px = img.load()
    for row in range(h):
        y = h - 1 - row  # DIB rows are stored bottom-up
        for x in range(w):
            byte = d[xor_off + row * xor_stride + x // 2]
            index = byte >> 4 if x % 2 == 0 else byte & 0x0F
            transparent = d[and_off + row * and_stride + x // 8] >> (7 - x % 8) & 1
            px[x, y] = (0, 0, 0, 0) if transparent else palette[index] + (255,)
    return img


def build_bitmaps():
    for name, src in BITMAPS.items():
        Image.open(RES / src).convert("RGB").save(ASSETS / f"{name}.png", optimize=True)

    # The splash screen is keyed on magenta (RGB 255,0,255).
    splash = Image.open(RES / "splash.bmp").convert("RGBA")
    px = splash.load()
    for y in range(splash.height):
        for x in range(splash.width):
            if px[x, y][:3] == (255, 0, 255):
                px[x, y] = (0, 0, 0, 0)
    splash.save(ASSETS / "splash.png", optimize=True)

    cursor_png(RES / "kursor256.cur").save(ASSETS / "kursor.png")

    icon = Image.open(RES / "Linije.ico").convert("RGBA")
    for size in (32, 180, 192, 512):
        icon.resize((size, size), Image.LANCZOS).save(ASSETS / f"icon-{size}.png", optimize=True)


def build_sounds():
    for name in SOUNDS:
        shutil.copyfile(ROOT / name, ASSETS / name)


def build_help():
    hh = shutil.which("hh.exe")
    if not hh:
        print("hh.exe not found, skipping help pages", file=sys.stderr)
        return
    out = WEB / "help"
    out.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        shutil.copyfile(ROOT / "Linije.chm", Path(tmp) / "Linije.chm")
        subprocess.run([hh, "-decompile", "out", "Linije.chm"], cwd=tmp, check=False)
        src = Path(tmp) / "out"
        for f in src.iterdir():
            if f.suffix.lower() == ".htm":
                html = f.read_bytes().decode("cp1250")
                html = re.sub(r"charset=windows-1250", "charset=utf-8", html, flags=re.I)
                (out / "index.html").write_text(html, encoding="utf-8")
            else:
                shutil.copyfile(f, out / f.name)


def main():
    ASSETS.mkdir(parents=True, exist_ok=True)
    build_sprites()
    build_bitmaps()
    build_sounds()
    build_help()
    print(f"assets written to {ASSETS.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
