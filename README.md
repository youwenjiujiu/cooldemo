# Island Grid — Singapore Energy 3D Story Animation

A fully animated, sci-fi styled 3D story (three.js + bloom post-processing) for a
Singapore-based renewable-energy IoT company. Neon wireframe Singapore coastline,
glowing tower assets, particle energy flows, cinematic text captions — **no stats,
no dashboards, pure narrative animation**.

## Run

```bash
python3 -m http.server 8000
# open http://localhost:8000/index.html
```

Needs network access only for the three.js CDN. `coast.js` (real OSM coastline of
Singapore Island, simplified) is bundled locally.

## The animation (~55 s, then endless slow orbit)

| Phase | What happens |
|-------|--------------|
| Outline | Neon coastline draws itself over a dark grid; brand title |
| City | Hundreds of dark city blocks rise from the island |
| Changi | Camera dives to Changi; an amber beacon ignites with orbiting fireflies; captions about real-time HVAC retuning |
| Assets | Pull back; client towers ignite one by one with name labels (fanned out around the CBD cluster) |
| Fabric | Violet energy arcs shoot from Changi to every asset; particles flow along them |
| Finale | Slow hero orbit; tagline + client name wall (text only); Replay |

Controls: `Skip ▸▸` (bottom-right) advances a phase; `Replay` restarts.

## Customize

- **Brand**: the `#title` block (`ISLAND GRID` placeholder) and `<title>`.
- **Sites**: the `SITES` constant (org label, lng/lat, tower height).
- **Captions**: the `CAPS` object — all narrative text lives there.
- **Pacing**: each phase's `dur` in the `PHASES` array.
- **Look**: bloom parameters (`UnrealBloomPass`), colors in `:root` CSS and materials.

`map-story.html` is the earlier MapLibre map-based 6-scene version, kept for reference.
