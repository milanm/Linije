// Offline support: serve from cache, refresh the cache in the background.
// Bump VERSION to drop old caches after a release.
const VERSION = 'linije-2';
const CORE = ['./', 'index.html', 'style.css', 'game.js', 'manifest.webmanifest', 'assets/sprites.json'];

self.addEventListener('install', (e) => {
  e.waitUntil(caches.open(VERSION).then((c) => c.addAll(CORE)).then(() => self.skipWaiting()));
});

self.addEventListener('activate', (e) => {
  e.waitUntil(
    caches.keys()
      .then((keys) => Promise.all(keys.filter((k) => k !== VERSION).map((k) => caches.delete(k))))
      .then(() => self.clients.claim()),
  );
});

self.addEventListener('fetch', (e) => {
  const req = e.request;
  // Leave the music to the network: Safari needs real range responses for audio.
  if (req.method !== 'GET' || req.headers.has('range') || req.url.endsWith('.mp3')) return;
  if (new URL(req.url).origin !== location.origin) return;
  e.respondWith(
    caches.open(VERSION).then(async (cache) => {
      const cached = await cache.match(req, { ignoreSearch: true });
      const fresh = fetch(req).then((res) => {
        if (res.ok) cache.put(req, res.clone());
        return res;
      });
      if (cached) {
        e.waitUntil(fresh.catch(() => {}));
        return cached;
      }
      return fresh;
    }),
  );
});
