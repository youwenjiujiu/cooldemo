# Univers · AIP for Physical Infrastructure — Singapore Story Animation

A fully animated cinematic demo on the **real Singapore map**: real 3D building
extrusions light up (Changi terminals in amber, CBD client towers in cyan), a mock
portfolio sweeps the whole island, and every asset weaves into an island-wide mesh
network. All narrative is **text-only captions — no numeric stats on screen**.

## Run

```bash
python3 -m http.server 8000
# open http://localhost:8000/index.html
```

## Basemap / token

`config.js` holds the Mapbox token:
- Token set (current) → **Mapbox navigation-night-v1**: bright readable night map,
  live traffic colors, detailed building footprints.
- Token empty → falls back to free **MapLibre + OpenFreeMap dark**, no token needed.

The `pk.` public token is standard frontend usage, but add a URL allowlist in the
Mapbox dashboard before publishing this repo anywhere public.

## The animation (~72 s, then endless slow orbit)

| Phase | What happens |
|-------|--------------|
| Intro | Island overview, "RUN AS ONE" title, first caption |
| Changi | Fly to Changi; the real T1/T2/T3 terminal buildings ignite amber; orbiting camera; captions about real-time HVAC retuning |
| CBD | Fly to Raffles Place / Marina Bay; real client towers ignite cyan (DBS, OCBC, NGS, Frasers, WITH) |
| Island | Pull back; 11 hero clients light one by one, then 24 mock assets sweep the island; a nearest-neighbour mesh (a web, not a hub) connects all 35 sites |
| Finale | "One ontology — every signal, one language" + text outcomes + client wall; slow endless orbit |

Controls: `Skip ▸▸` advances a phase; `Replay` restarts.

## Customize

- **Clients**: `SITES` (hero, big dots + finale wall) and `MOCK_SITES` (island
  coverage, small dots) at the top of `index.html`.
- **Captions / copy**: the `CAPS` object, `#title` and `#finale` markup.
- **Pacing**: each phase's `dur` in `PHASES`.
- **Building lighting**: `lightBuildingsAt(coords, tone, radiusPx, maxMeters)` calls
  in the Changi/CBD phases.

## Other versions kept for reference

- `neon-story.html` — stylized three.js neon 3D version (self-contained, no map
  tiles; also published as a claude.ai artifact).
- `map-story.html` — earliest MapLibre dashboard-style version with HVAC panel and
  numeric stats.

## Unreal 5 interactive twin (in progress)

`unreal/` holds the UE 5.5 + Cesium project scaffold for the clickable, game-feel
version: photorealistic Google 3D Tiles Singapore, free-fly + clickable assets,
the same three storylines. Start with `unreal/README-SETUP.md`; architecture and
roadmap in `unreal/DESIGN.md`. Site/fault data in `unreal/Content/Data/` mirrors
the web version's `SITES`/`FAULTS`.

## twin.html — 浏览器版交互式数字孪生（主力版本）

three.js + Google Photorealistic 3D Tiles（经 Cesium ion，复用同一 token）。
开场三章节自动导览（Energy Efficiency → Energy Management → IoC·FDD）后进入自由探索：
拖动环视、WASD 飞行、滚轮调速、点击光柱弹资产卡、FDD 故障事件周期性上演、R 回起点。

运行：项目根目录建 `keys.js`（gitignored）：`const ION_TOKEN = "你的 ion token";`
然后 `python3 -m http.server 8000` 打开 `/twin.html`。

UE5 版（unreal/）保留为展厅级备选。
