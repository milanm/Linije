// Offline support. Pages and code come from the network first so updates
// show up right away; images and sounds come from the cache first.
// Bump VERSION to drop old caches.
const VERSION = 'linije-3';
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
  const url = new URL(req.url);
  // Leave the music to the network: Safari needs real range responses for audio.
  if (req.method !== 'GET' || req.headers.has('range') || url.pathname.endsWith('.mp3')) return;
  if (url.origin !== location.origin) return;

  const media = /\.(png|jpg|gif|wav)$/i.test(url.pathname);
  e.respondWith(
    caches.open(VERSION).then(async (cache) => {
      const fromNetwork = fetch(req).then((res) => {
        if (res.ok) cache.put(req, res.clone());
        return res;
      });
      if (media) {
        const cached = await cache.match(req);
        if (cached) {
          e.waitUntil(fromNetwork.catch(() => {}));
          return cached;
        }
        return fromNetwork;
      }
      try {
        return await fromNetwork;
      } catch {
        return (await cache.match(req, { ignoreSearch: true })) || Response.error();
      }
    }),
  );
});
