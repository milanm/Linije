// Linije 1.0 for the web. Game rules, timings and layout follow the 2004
// MFC original (LinijeDlg.cpp); all coordinates below are pixels in the
// original 671x544 window.
'use strict';

(() => {
  const W0 = 671;
  const H0 = 544;
  const N = 10;
  const CELL = { x0: 53, y0: 93, dx: 39, dy: 40, w: 37, h: 38 };
  const BOARD = { x1: 53, y1: 93, x2: 445, y2: 491 };
  const SHAPES = ['kugla', 'cube', 'romb', 'trougao', 'zvezda'];
  const WALL = 'zidic';
  const NEXT_POS = [[508, 200], [551, 200], [594, 200]];
  const SCORE_RECT = [526, 110, 92, 25];
  // Shape counters at the bottom right, in the order of the icons above them.
  const COUNTERS = [
    ['kugla', 514, 443, 16, 12],
    ['cube', 540, 443, 15, 12],
    ['trougao', 564, 443, 15, 12],
    ['zvezda', 591, 443, 14, 12],
    ['romb', 616, 443, 15, 12],
  ];
  const BUTTONS = {
    meni: [54, 44, 107, 23],
    sledece: [588, 312, 46, 21],
    sistemski: [20, 7, 22, 18],
    min: [603, 7, 22, 18],
    x: [629, 7, 22, 18],
  };
  const TOOLTIPS = { x: 'Close', min: 'Minimize', sledece: 'Sledeće figure' };
  const MENU_POS = [54, 67];
  const FRAME_MS = 30;
  const MAX_SCALE = 2;

  // Wide is the original window. Tall stacks the right-hand panel under the
  // board so the cells stay big enough to tap on a phone held upright.
  // Rects are [sx, sy, w, h, dx, dy]: regions copy from the live scene,
  // fills and patches copy plain stone from the background bitmap.
  const WIDE = { w: W0, h: H0, fills: [], regions: [[0, 0, W0, H0, 0, 0]], patches: [] };
  const TALL = {
    w: 455,
    h: 796,
    fills: [[456, 48, 11, 252, 0, 544], [659, 48, 12, 252, 443, 544]],
    regions: [[0, 0, 455, 544, 0, 0], [455, 48, 216, 252, 11, 544], [455, 292, 216, 252, 227, 544]],
    patches: [[452, 405, 3, 60, 452, 470]],
  };

  const SOUNDS = { dodaj: 'AddBall', bonus: 'bonus', kraj: 'GameOver', klik: 'loptica', brisi: 'DelBall' };
  const KEYS = { settings: 'linije.podesavanja', save: 'linije.sav', scores: 'linije.rez', current: 'linije.igra' };

  const $ = (sel) => document.querySelector(sel);
  const stage = $('#stage');
  const inner = $('#inner');
  const canvas = $('#screen');
  const ctx = canvas.getContext('2d');
  const scene = document.createElement('canvas');
  scene.width = W0;
  scene.height = H0;
  const sctx = scene.getContext('2d');

  const IMG = {};
  let SPRITES = {};
  let layout = WIDE;
  let scale = 1;
  let dirty = true;
  let ready = false;
  let started = false;
  let modal = null;

  const store = {
    get(key, fallback) {
      try {
        const v = localStorage.getItem(key);
        return v == null ? fallback : JSON.parse(v);
      } catch {
        return fallback;
      }
    },
    set(key, value) {
      try { localStorage.setItem(key, JSON.stringify(value)); } catch { /* private mode */ }
    },
  };

  const settings = Object.assign({ muzika: true, prepreke: false }, store.get(KEYS.settings, {}));
  const saveSettings = () => store.set(KEYS.settings, settings);

  const game = {
    grid: emptyGrid(),
    next: [],
    score: 0,
    selected: null,
    hover: null,
    moving: null,
    booms: [],
    busy: false,
    sledeceOn: true,
    pressed: null,
    hoverBtn: null,
  };

  function emptyGrid() {
    return Array.from({ length: N }, () => Array(N).fill(null));
  }

  const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
  const rnd = (n) => Math.floor(Math.random() * n);
  const inRect = (x, y, r) => x >= r[0] && x < r[0] + r[2] && y >= r[1] && y < r[1] + r[3];
  const redraw = () => { dirty = true; };

  // ---------- Assets ----------

  function loadImage(name) {
    return new Promise((resolve, reject) => {
      const img = new Image();
      img.onload = () => resolve(img);
      img.onerror = () => reject(new Error(`assets/${name}.png`));
      img.src = `assets/${name}.png`;
    });
  }

  async function loadAssets() {
    SPRITES = await fetch('assets/sprites.json').then((r) => r.json());
    const names = ['tabla', 'meni', 'sledece', 'sledece-klik', 'sledece-neaktivno',
      'x', 'x-preko', 'x-klik', 'min-neaktivno', 'sistemski', ...Object.keys(SPRITES)];
    await Promise.all(names.map(async (n) => { IMG[n] = await loadImage(n); }));
    ready = true;
  }

  // ---------- Sound ----------

  let actx = null;
  const soundData = {};
  const soundBuf = {};

  function fetchSounds() {
    return Promise.all(Object.entries(SOUNDS).map(async ([key, file]) => {
      try {
        soundData[key] = await fetch(`assets/${file}.wav`).then((r) => r.arrayBuffer());
      } catch { /* play without this sound */ }
    }));
  }

  // Browsers only allow audio after a tap or click, so this runs on the first one.
  function unlockAudio() {
    if (actx) return;
    const AC = window.AudioContext || window.webkitAudioContext;
    if (!AC) return;
    actx = new AC();
    for (const [key, data] of Object.entries(soundData)) {
      new Promise((res, rej) => actx.decodeAudioData(data.slice(0), res, rej))
        .then((buf) => { soundBuf[key] = buf; })
        .catch(() => {});
    }
  }

  // Like the original, sound effects follow the "Muzika" switch.
  function playSound(key) {
    if (!settings.muzika || !actx || !soundBuf[key]) return;
    if (actx.state === 'suspended') actx.resume();
    const src = actx.createBufferSource();
    src.buffer = soundBuf[key];
    src.connect(actx.destination);
    src.start();
  }

  const music = new Audio();
  music.loop = true;
  music.preload = 'none';
  music.src = 'assets/zvuk.mp3';

  function updateMusic() {
    if (settings.muzika && started && !document.hidden) music.play().catch(() => {});
    else music.pause();
  }

  // ---------- Rules ----------

  const freeCells = () => {
    const out = [];
    for (let x = 0; x < N; x++) for (let y = 0; y < N; y++) if (!game.grid[x][y]) out.push({ x, y });
    return out;
  };

  const randomNext = () => [0, 1, 2].map(() => SHAPES[rnd(SHAPES.length)]);

  function newGame() {
    game.grid = emptyGrid();
    game.score = 0;
    game.selected = null;
    game.moving = null;
    game.booms = [];
    if (settings.prepreke) addWalls(5);
    if (game.next.length !== 3) game.next = randomNext();
    spawn(3);
    game.sledeceOn = true;
    autosave();
    redraw();
  }

  function spawn(n) {
    const free = freeCells();
    const count = Math.min(n, free.length);
    for (let i = 0; i < count; i++) {
      const c = free.splice(rnd(free.length), 1)[0];
      game.grid[c.x][c.y] = game.next[i];
    }
    game.next = randomNext();
    playSound('dodaj');
    redraw();
  }

  function addWalls(n) {
    const free = freeCells();
    for (let i = 0; i < n && free.length; i++) {
      const c = free.splice(rnd(free.length), 1)[0];
      game.grid[c.x][c.y] = WALL;
    }
  }

  function removeWalls() {
    for (let x = 0; x < N; x++) for (let y = 0; y < N; y++) if (game.grid[x][y] === WALL) game.grid[x][y] = null;
  }

  // A* from the original: eight directions (cost 10 straight, 14 diagonal),
  // but a diagonal step may not cut past an occupied neighbour.
  function findPath(from, to) {
    const key = (x, y) => y * N + x;
    const blocked = (x, y) => x < 0 || y < 0 || x >= N || y >= N || game.grid[x][y] !== null;
    const h = (x, y) => 10 * (Math.abs(x - to.x) + Math.abs(y - to.y));
    const g = new Map([[key(from.x, from.y), 0]]);
    const parent = new Map();
    const open = [{ x: from.x, y: from.y, f: h(from.x, from.y) }];
    const closed = new Set();
    while (open.length) {
      open.sort((a, b) => a.f - b.f);
      const cur = open.shift();
      const ck = key(cur.x, cur.y);
      if (closed.has(ck)) continue;
      if (cur.x === to.x && cur.y === to.y) break;
      closed.add(ck);
      for (let dy = -1; dy <= 1; dy++) {
        for (let dx = -1; dx <= 1; dx++) {
          if (!dx && !dy) continue;
          const nx = cur.x + dx;
          const ny = cur.y + dy;
          if (blocked(nx, ny) || closed.has(key(nx, ny))) continue;
          if (dx && dy && (blocked(cur.x + dx, cur.y) || blocked(cur.x, cur.y + dy))) continue;
          const cost = g.get(ck) + (dx && dy ? 14 : 10);
          const nk = key(nx, ny);
          if (g.has(nk) && g.get(nk) <= cost) continue;
          g.set(nk, cost);
          parent.set(nk, ck);
          open.push({ x: nx, y: ny, f: cost + h(nx, ny) });
        }
      }
    }
    const end = key(to.x, to.y);
    if (!parent.has(end)) return null;
    const path = [];
    for (let k = end; k !== key(from.x, from.y); k = parent.get(k)) path.unshift({ x: k % N, y: Math.floor(k / N) });
    return path;
  }

  // Every run of five or more equal shapes, horizontally, vertically or diagonally.
  function findLines() {
    const runs = [];
    const dirs = [[1, 0], [0, 1], [1, 1], [1, -1]];
    for (const [dx, dy] of dirs) {
      for (let x = 0; x < N; x++) {
        for (let y = 0; y < N; y++) {
          const shape = game.grid[x][y];
          if (!shape || shape === WALL) continue;
          const px = x - dx;
          const py = y - dy;
          if (px >= 0 && py >= 0 && px < N && py < N && game.grid[px][py] === shape) continue;
          const cells = [];
          for (let cx = x, cy = y; cx >= 0 && cy >= 0 && cx < N && cy < N && game.grid[cx][cy] === shape; cx += dx, cy += dy) {
            cells.push({ x: cx, y: cy });
          }
          if (cells.length >= 5) runs.push({ shape, cells });
        }
      }
    }
    return runs;
  }

  async function clearRuns(runs) {
    const t = performance.now();
    let longest = 0;
    for (const run of runs) {
      const extra = (run.cells.length - 5) * 50;
      game.score += 100 + extra;
      if (extra) showBonus(` ${extra} poena`);
      longest = Math.max(longest, run.cells.length);
      for (const c of run.cells) {
        if (!game.grid[c.x][c.y]) continue;
        game.booms.push({ shape: run.shape, x: c.x, y: c.y, t });
        game.grid[c.x][c.y] = null;
      }
    }
    playSound(longest > 5 ? 'bonus' : 'brisi');
    redraw();
    await sleep(Math.max(...game.booms.map((b) => SPRITES[`${b.shape}-boom`].frames)) * FRAME_MS);
    game.booms = [];
    redraw();
  }

  async function move(path) {
    game.busy = true;
    const { x: fx, y: fy } = game.selected;
    const shape = game.grid[fx][fy];
    game.selected = null;
    game.grid[fx][fy] = null;
    const pause = path.length < 5 ? 130 : path.length <= 10 ? 120 : 110;
    game.moving = { shape, x: fx, y: fy };
    for (const c of path) {
      game.moving.x = c.x;
      game.moving.y = c.y;
      redraw();
      await sleep(pause);
    }
    game.moving = null;
    const end = path[path.length - 1];
    game.grid[end.x][end.y] = shape;

    let runs = findLines();
    if (runs.length) {
      await clearRuns(runs);
    } else {
      spawn(3);
      while ((runs = findLines()).length) await clearRuns(runs);
    }
    game.sledeceOn = true;
    autosave();
    if (!freeCells().length) await gameOver();
    game.busy = false;
    redraw();
  }

  function clickCell(x, y) {
    if (game.busy) return;
    const shape = game.grid[x][y];
    if (shape === WALL) return;
    if (shape) {
      playSound('klik');
      const sel = game.selected;
      game.selected = sel && sel.x === x && sel.y === y ? null : { x, y, t: performance.now() };
      redraw();
      return;
    }
    if (!game.selected) return;
    const path = findPath(game.selected, { x, y });
    if (!path) {
      game.selected = null;
      redraw();
      return;
    }
    move(path);
  }

  function rerollNext() {
    if (!game.sledeceOn || game.busy) return;
    game.next = randomNext();
    game.sledeceOn = false;
    autosave();
    redraw();
  }

  async function gameOver() {
    playSound('kraj');
    const entered = await checkHighScore(game.score);
    if (!entered) await message('Igra je zavrsena!');
    newGame();
  }

  // ---------- Saving ----------

  const flatGrid = () => game.grid.flat();
  const loadGrid = (flat) => Array.from({ length: N }, (_, x) => flat.slice(x * N, x * N + N));

  function autosave() {
    store.set(KEYS.current, { score: game.score, grid: flatGrid(), next: game.next, sledeceOn: game.sledeceOn });
  }

  function restore() {
    const s = store.get(KEYS.current, null);
    if (!s || !Array.isArray(s.grid) || s.grid.length !== N * N) return false;
    game.grid = loadGrid(s.grid);
    game.score = s.score | 0;
    game.next = Array.isArray(s.next) && s.next.length === 3 ? s.next : randomNext();
    game.sledeceOn = s.sledeceOn !== false;
    return true;
  }

  function saveGame() {
    store.set(KEYS.save, { score: game.score, grid: flatGrid() });
  }

  function loadGame() {
    const s = store.get(KEYS.save, null);
    if (!s || !Array.isArray(s.grid)) return false;
    game.grid = loadGrid(s.grid);
    game.score = s.score | 0;
    game.next = randomNext();
    game.selected = null;
    settings.prepreke = s.grid.includes(WALL);
    saveSettings();
    autosave();
    redraw();
    return true;
  }

  function loadScores() {
    const s = store.get(KEYS.scores, null);
    if (Array.isArray(s) && s.length === 10) return s;
    return Array.from({ length: 10 }, () => ({ ime: 'Prazno', rezultat: 0 }));
  }

  async function checkHighScore(score) {
    if (!score) return false;
    const scores = loadScores();
    const i = scores.findIndex((s) => score > s.rezultat);
    if (i < 0) return false;
    const ime = (await askName()) || 'Prazno';
    scores.splice(i, 0, { ime, rezultat: score });
    scores.length = 10;
    store.set(KEYS.scores, scores);
    await showScores(i);
    return true;
  }

  // ---------- Drawing ----------

  function sprite(name, frame, dx, dy, w, h) {
    const meta = SPRITES[name];
    const img = IMG[name];
    if (!meta || !img) return;
    const f = frame % meta.frames;
    sctx.drawImage(img, f * meta.w, 0, w || meta.w, h || meta.h, dx, dy, w || meta.w, h || meta.h);
  }

  const cellX = (x) => CELL.x0 + CELL.dx * x;
  const cellY = (y) => CELL.y0 + CELL.dy * y;

  function drawPiece(shape, x, y, now) {
    if (shape === WALL) return sprite(WALL, 0, cellX(x), cellY(y));
    const sel = game.selected;
    if (sel && sel.x === x && sel.y === y) {
      return sprite(`${shape}-anim`, Math.floor((now - sel.t) / FRAME_MS), cellX(x), cellY(y), CELL.w, CELL.h);
    }
    const hover = game.hover && game.hover.x === x && game.hover.y === y && !game.busy;
    sprite(hover ? `${shape}-over` : shape, 0, cellX(x), cellY(y), CELL.w, CELL.h);
  }

  function drawScene(now) {
    const c = sctx;
    c.drawImage(IMG.tabla, 0, 0);

    c.drawImage(IMG.sistemski, BUTTONS.sistemski[0], BUTTONS.sistemski[1]);
    c.drawImage(IMG['min-neaktivno'], BUTTONS.min[0], BUTTONS.min[1]);
    const xImg = game.pressed === 'x' ? 'x-klik' : game.hoverBtn === 'x' ? 'x-preko' : 'x';
    c.drawImage(IMG[xImg], BUTTONS.x[0], BUTTONS.x[1]);
    // FooButton draws the bitmap one pixel in from its left edge.
    const [mx, my, mw, mh] = BUTTONS.meni;
    c.drawImage(IMG.meni, 1, 0, mw, mh, mx, my, mw, mh);
    const sl = !game.sledeceOn ? 'sledece-neaktivno' : game.pressed === 'sledece' ? 'sledece-klik' : 'sledece';
    c.drawImage(IMG[sl], BUTTONS.sledece[0], BUTTONS.sledece[1]);

    const [sx, sy, sw, sh] = SCORE_RECT;
    c.fillStyle = '#000';
    c.fillRect(sx, sy, sw, sh);
    c.fillStyle = '#0f0';
    c.font = 'bold 19px "Trebuchet MS", "Lucida Grande", "Lucida Sans Unicode", sans-serif';
    c.textAlign = 'center';
    c.textBaseline = 'alphabetic';
    c.letterSpacing = '1px';
    c.fillText(String(game.score), sx + sw / 2 + 1, sy + 20);
    c.letterSpacing = '0px';
    c.textBaseline = 'middle';

    const counts = {};
    for (const col of game.grid) for (const s of col) if (s) counts[s] = (counts[s] || 0) + 1;
    c.font = '10px Tahoma, Verdana, "Segoe UI", sans-serif';
    for (const [shape, x, y, w, h] of COUNTERS) {
      c.fillStyle = 'rgb(180,178,178)';
      c.fillRect(x, y, w, h);
      c.fillStyle = '#000';
      c.fillText(String(counts[shape] || 0), x + w / 2, y + h / 2 + 1);
    }

    game.next.forEach((shape, i) => sprite(shape, 0, NEXT_POS[i][0], NEXT_POS[i][1]));

    for (let x = 0; x < N; x++) {
      for (let y = 0; y < N; y++) {
        const shape = game.grid[x][y];
        if (shape) drawPiece(shape, x, y, now);
      }
    }
    if (game.moving) sprite(game.moving.shape, 0, cellX(game.moving.x), cellY(game.moving.y), CELL.w, CELL.h);
    for (const b of game.booms) {
      const f = Math.floor((now - b.t) / FRAME_MS);
      if (f < SPRITES[`${b.shape}-boom`].frames) sprite(`${b.shape}-boom`, f, cellX(b.x), cellY(b.y), CELL.w, CELL.h);
    }
  }

  function render(now) {
    drawScene(now);
    const k = scale * (window.devicePixelRatio || 1);
    ctx.setTransform(k, 0, 0, k, 0, 0);
    ctx.imageSmoothingEnabled = true;
    ctx.imageSmoothingQuality = 'high';
    ctx.fillStyle = '#3c4256';
    ctx.fillRect(0, 0, layout.w, layout.h);
    for (const [sx, sy, w, h, dx, dy] of layout.fills) ctx.drawImage(IMG.tabla, sx, sy, w, h, dx, dy, w, h);
    for (const [sx, sy, w, h, dx, dy] of layout.regions) ctx.drawImage(scene, sx, sy, w, h, dx, dy, w, h);
    for (const [sx, sy, w, h, dx, dy] of layout.patches) ctx.drawImage(IMG.tabla, sx, sy, w, h, dx, dy, w, h);
  }

  function loop(now) {
    requestAnimationFrame(loop);
    if (ready && (dirty || game.selected || game.booms.length)) {
      dirty = false;
      render(now);
    }
  }

  // ---------- Layout ----------

  function fit() {
    const vv = window.visualViewport;
    const vw = vv ? vv.width : window.innerWidth;
    const footer = $('#footer');
    const footerH = footer.offsetHeight || 0;
    const vh = (vv ? vv.height : window.innerHeight) - footerH;
    const m = 8;
    const sWide = Math.min((vw - 2 * m) / WIDE.w, (vh - 2 * m) / WIDE.h);
    const sTall = Math.min((vw - 2 * m) / TALL.w, (vh - 2 * m) / TALL.h);
    layout = sTall > sWide * 1.1 ? TALL : WIDE;
    scale = Math.min(layout === TALL ? sTall : sWide, MAX_SCALE);

    const w = layout.w * scale;
    const h = layout.h * scale;
    stage.style.width = `${w}px`;
    stage.style.height = `${h}px`;
    stage.style.left = `${Math.max(0, (vw - w) / 2)}px`;
    stage.style.top = `${Math.max(0, (vh - h) / 2)}px`;
    inner.style.width = `${layout.w}px`;
    inner.style.height = `${layout.h}px`;
    inner.style.transform = `scale(${scale})`;
    canvas.style.width = `${layout.w}px`;
    canvas.style.height = `${layout.h}px`;
    const dpr = window.devicePixelRatio || 1;
    canvas.width = Math.round(w * dpr);
    canvas.height = Math.round(h * dpr);

    const splash = $('#splash');
    splash.style.transform = `translate(-50%, -50%) scale(${Math.min(Math.max(scale, 0.8), 1.5)})`;
    for (const el of document.querySelectorAll('.dialog, .msgbox')) if (!el.hidden) center(el);
    redraw();
  }

  // Layout point to original window point, or null outside every region.
  function toOriginal(e) {
    const r = canvas.getBoundingClientRect();
    const lx = ((e.clientX - r.left) / r.width) * layout.w;
    const ly = ((e.clientY - r.top) / r.height) * layout.h;
    for (const [sx, sy, w, h, dx, dy] of layout.regions) {
      if (lx >= dx && lx < dx + w && ly >= dy && ly < dy + h) return [lx - dx + sx, ly - dy + sy];
    }
    return null;
  }

  function hitButton(p) {
    if (!p) return null;
    for (const [name, r] of Object.entries(BUTTONS)) if (inRect(p[0], p[1], r)) return name;
    return null;
  }

  function hitCell(p) {
    if (!p) return null;
    const [x, y] = p;
    if (x <= BOARD.x1 || x >= BOARD.x2 || y <= BOARD.y1 || y >= BOARD.y2) return null;
    return {
      x: Math.min(N - 1, Math.max(0, Math.floor((x - CELL.x0) / CELL.dx))),
      y: Math.min(N - 1, Math.max(0, Math.floor((y - CELL.y0) / CELL.dy))),
    };
  }

  // ---------- Input ----------

  canvas.addEventListener('pointerdown', (e) => {
    if (!started || modal) return;
    if (!$('#menu').hidden) { closeMenu(); return; }
    const p = toOriginal(e);
    const btn = hitButton(p);
    if (btn === 'meni' || btn === 'sistemski') { openMenu(); return; }
    if (btn === 'x' || btn === 'sledece') {
      game.pressed = btn;
      canvas.setPointerCapture(e.pointerId);
      redraw();
      return;
    }
    const cell = hitCell(p);
    if (cell) clickCell(cell.x, cell.y);
  });

  canvas.addEventListener('pointerup', (e) => {
    const pressed = game.pressed;
    if (!pressed) return;
    game.pressed = null;
    redraw();
    if (hitButton(toOriginal(e)) !== pressed) return;
    if (pressed === 'sledece') rerollNext();
    if (pressed === 'x') izlaz();
  });

  canvas.addEventListener('pointercancel', () => { game.pressed = null; redraw(); });

  canvas.addEventListener('pointermove', (e) => {
    if (e.pointerType !== 'mouse') return;
    const p = toOriginal(e);
    const btn = hitButton(p);
    const cell = hitCell(p);
    const piece = cell && game.grid[cell.x][cell.y] && game.grid[cell.x][cell.y] !== WALL ? cell : null;
    if (btn !== game.hoverBtn || JSON.stringify(piece) !== JSON.stringify(game.hover)) {
      game.hoverBtn = btn;
      game.hover = piece;
      canvas.title = TOOLTIPS[btn] || '';
      redraw();
    }
  });

  canvas.addEventListener('pointerleave', () => {
    game.hover = null;
    game.hoverBtn = null;
    redraw();
  });

  document.addEventListener('keydown', (e) => {
    if (e.key === 'F1') { e.preventDefault(); openHelp(); }
    if (e.key === 'Escape') {
      if (!$('#menu').hidden) closeMenu();
      else if (modal) modal.close();
    }
  });

  document.addEventListener('visibilitychange', updateMusic);
  window.addEventListener('resize', fit);
  if (window.visualViewport) window.visualViewport.addEventListener('resize', fit);

  // ---------- Menu ----------

  const menu = $('#menu');

  function openMenu() {
    menu.querySelector('[data-cmd="ucitaj"]').disabled = !store.get(KEYS.save, null);
    menu.querySelector('[data-cmd="muzika"]').setAttribute('aria-checked', String(settings.muzika));
    menu.querySelector('[data-cmd="prepreke"]').setAttribute('aria-checked', String(settings.prepreke));
    menu.style.left = `${MENU_POS[0]}px`;
    menu.style.top = `${MENU_POS[1]}px`;
    menu.hidden = false;
  }

  function closeMenu() { menu.hidden = true; }

  document.addEventListener('pointerdown', (e) => {
    if (!menu.hidden && !menu.contains(e.target) && e.target !== canvas) closeMenu();
  });

  menu.addEventListener('click', (e) => {
    const item = e.target.closest('button');
    if (!item || item.disabled) return;
    closeMenu();
    command(item.dataset.cmd);
  });

  async function command(cmd) {
    if (game.busy && ['nova', 'sacuvaj', 'ucitaj', 'prepreke', 'izlaz'].includes(cmd)) return;
    switch (cmd) {
      case 'nova': newGame(); break;
      case 'sacuvaj': saveGame(); await message('Igra je sacuvana !'); break;
      case 'ucitaj': if (!loadGame()) await message('Igra nije sacuvana !'); break;
      case 'kopiraj': await copyBoard(); break;
      case 'muzika':
        settings.muzika = !settings.muzika;
        saveSettings();
        updateMusic();
        break;
      case 'prepreke':
        settings.prepreke = !settings.prepreke;
        if (settings.prepreke) addWalls(5); else removeWalls();
        saveSettings();
        autosave();
        redraw();
        break;
      case 'rezultati': await showScores(-1); break;
      case 'internet': window.open('https://github.com/milanm/Linije', '_blank', 'noopener'); break;
      case 'pomoc': openHelp(); break;
      case 'oigri': await about(); break;
      case 'izlaz': await izlaz(); break;
      default: break;
    }
  }

  // The web can't close the window, so "Izlaz" ends the game instead:
  // record the score like the original did on exit, then start over.
  async function izlaz() {
    if (game.busy) return;
    const entered = await checkHighScore(game.score);
    if (!entered) await message('Igra je zavrsena!');
    newGame();
  }

  function openHelp() {
    window.open('help/index.html', '_blank', 'noopener');
  }

  async function copyBoard() {
    drawScene(performance.now());
    const blob = await new Promise((r) => scene.toBlob(r, 'image/png'));
    try {
      await navigator.clipboard.write([new ClipboardItem({ 'image/png': blob })]);
    } catch {
      const a = document.createElement('a');
      a.href = URL.createObjectURL(blob);
      a.download = 'linije.png';
      a.click();
      setTimeout(() => URL.revokeObjectURL(a.href), 1000);
    }
  }

  // ---------- Dialogs ----------

  function center(el) {
    el.style.left = `${Math.round((layout.w - el.offsetWidth) / 2)}px`;
    el.style.top = `${Math.round((layout.h - el.offsetHeight) / 2)}px`;
  }

  function openModal(el, onClose) {
    return new Promise((resolve) => {
      el.hidden = false;
      center(el);
      const close = (value) => {
        el.hidden = true;
        modal = null;
        if (onClose) onClose();
        resolve(value);
      };
      modal = { el, close };
    });
  }

  function message(text) {
    const box = $('#dlg-poruka');
    box.querySelector('p').textContent = text;
    const done = openModal(box);
    box.querySelector('.msgbox-ok').focus();
    return done;
  }

  $('#dlg-poruka .msgbox-ok').addEventListener('click', () => modal && modal.close());
  $('#dlg-poruka .msgbox-x').addEventListener('click', () => modal && modal.close());

  function showScores(selected) {
    const dlg = $('#dlg-rezultati');
    const body = dlg.querySelector('tbody');
    body.replaceChildren(...loadScores().map((s, i) => {
      const tr = document.createElement('tr');
      if (i === selected) tr.className = 'izabran';
      for (const v of [i + 1, s.ime, s.rezultat]) {
        const td = document.createElement('td');
        td.textContent = v;
        tr.append(td);
      }
      return tr;
    }));
    const done = openModal(dlg);
    const row = body.querySelector('.izabran');
    if (row) row.scrollIntoView({ block: 'nearest' });
    return done;
  }

  for (const b of document.querySelectorAll('#dlg-rezultati button, #dlg-oigri button')) {
    b.addEventListener('click', () => modal && modal.close());
  }

  function askName() {
    const form = $('#dlg-unos');
    const input = form.elements.ime;
    input.value = '';
    const done = openModal(form);
    setTimeout(() => input.focus(), 0);
    return done;
  }

  $('#dlg-unos').addEventListener('submit', (e) => {
    e.preventDefault();
    if (modal) modal.close($('#dlg-unos').elements.ime.value.trim());
  });

  function about() { return openModal($('#dlg-oigri')); }

  // ---------- Bonus notification ----------

  let bonusTimer = 0;
  function showBonus(text) {
    const el = $('#bonus');
    el.querySelector('span').textContent = text;
    el.classList.add('vidljiv');
    clearTimeout(bonusTimer);
    bonusTimer = setTimeout(() => el.classList.remove('vidljiv'), 3500);
  }

  // ---------- Start ----------

  async function start() {
    fit();
    const splash = $('#splash');
    const text = $('#splash-text');
    requestAnimationFrame(() => splash.classList.add('vidljiv'));
    requestAnimationFrame(loop);

    const shown = sleep(900);
    const sounds = fetchSounds();
    try {
      await loadAssets();
    } catch (err) {
      text.textContent = 'Greška pri učitavanju.';
      throw err;
    }
    text.textContent = 'Učitavam podatke...';
    if (!restore()) newGame();
    redraw();
    await sleep(300);
    text.textContent = 'Učitavam muziku...';
    await Promise.all([sounds, shown]);

    text.textContent = 'Kliknite za početak';
    splash.classList.add('ceka');
    await new Promise((resolve) => {
      const go = (e) => {
        if (e.type === 'keydown' && !['Enter', ' '].includes(e.key)) return;
        window.removeEventListener('pointerdown', go);
        window.removeEventListener('keydown', go);
        // iOS only starts audio from inside the tap handler itself.
        unlockAudio();
        started = true;
        updateMusic();
        resolve();
      };
      window.addEventListener('pointerdown', go);
      window.addEventListener('keydown', go);
    });

    splash.classList.remove('vidljiv', 'ceka');
    setTimeout(() => { splash.hidden = true; }, 450);
  }

  start();

  if ('serviceWorker' in navigator && location.protocol === 'https:') {
    navigator.serviceWorker.register('sw.js').catch(() => {});
  }
})();
