# Island Grid — Singapore Energy IoT Story Animation

A single-file cinematic story demo for a Singapore-based renewable-energy IoT company:
the camera flies across Singapore, lights up Changi Airport T3 with a live HVAC
optimization panel, reveals 11 client assets, weaves them into an island-wide energy
network, and lands on an outcomes card.

## Run

```bash
python3 -m http.server 8000
# open http://localhost:8000/index.html
```

(Or just double-click `index.html` — everything is inline; only the map tiles and
MapLibre CDN need network access.)

## Storyline (66 s auto-play)

| # | Scene | What happens |
|---|-------|--------------|
| 1 | Intro | Island overview, brand title fades in |
| 2 | Changi HVAC | Fly to T3, building blooms, live ops panel (flights/hr, pax flow, chiller kW, setpoint, AI knowledge-topology graph) |
| 3 | Algorithm | Convergence chart: jittery baseline → optimized curve, savings count-up |
| 4 | Constellation | Pull up; 11 client assets light up one by one |
| 5 | Network | Energy arcs shoot from Changi to every asset, flowing dashes |
| 6 | Outcomes | Stats count-up + client wall (Changi, DBS, OCBC, HTX, MSE, NGS, ST Engg, HDB, PSA, Frasers, WITH) |

Controls: prev / play-pause / next, scene dots, Replay, and **Explore map** (frees the camera).

## Customize

- **Brand**: edit the `#intro` block (`ISLAND GRID` placeholder) and `<title>`.
- **Sites / outcomes**: the `SITES` and `OUTCOMES` constants at the top of the script.
- **Basemap**: currently MapLibre + OpenFreeMap dark (no token needed). To use Mapbox,
  swap the script/CSS includes for mapbox-gl, set `mapboxgl.accessToken`, and change the
  `style` URL — the camera/layer API is compatible.
- **Timings**: each scene's `dur` in the `SCENES` array.
