"""
Auto-build the SingaporeTwin level: Cesium georeference + sky + Singapore tiles
+ 35 asset beacons from Sites.csv.

Run from the repo root:
  "/Users/Shared/Epic Games/UE_5.5/Engine/Binaries/Mac/UnrealEditor-Cmd" \
    unreal/IslandGrid.uproject -run=pythonscript \
    -script="$(pwd)/unreal/Tools/setup_singapore_twin.py"

Or inside the editor: Output Log -> Cmd dropdown -> Python, paste:
  py "/Users/youwen/cooldemo/unreal/Tools/setup_singapore_twin.py"

Tokens: put them in unreal/keys.json (gitignored):
  { "cesium_ion_token": "...", "google_tiles_key": "..." }
With a google key -> Google Photorealistic 3D Tiles; else with an ion token ->
Cesium World Terrain + OSM Buildings; with neither the tileset is left for later.
"""
import json
import os
import csv

import unreal

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LEVEL_PATH = "/Game/Maps/SingaporeTwin"
ORIGIN = (1.3400, 103.8200, 250.0)  # lat, lng, height
SITES_CSV = os.path.join(ROOT, "Content", "Data", "Sites.csv")
KEYS_JSON = os.path.join(ROOT, "keys.json")


def load_keys():
    if os.path.exists(KEYS_JSON):
        with open(KEYS_JSON) as f:
            return json.load(f)
    return {}


def get_or_create_level():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if not unreal.EditorAssetLibrary.does_asset_exist(LEVEL_PATH):
        les.new_level(LEVEL_PATH)
        unreal.log("Created level " + LEVEL_PATH)
    else:
        les.load_level(LEVEL_PATH)
        unreal.log("Loaded existing level " + LEVEL_PATH)


def spawn(cls, label):
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for a in eas.get_all_level_actors():
        if a.get_actor_label() == label:
            unreal.log(label + " already present")
            return a
    actor = eas.spawn_actor_from_class(cls, unreal.Vector(0, 0, 0))
    actor.set_actor_label(label)
    return actor


def setup_cesium(keys):
    geo = spawn(unreal.CesiumGeoreference, "CesiumGeoreference")
    geo.set_editor_property("origin_latitude", ORIGIN[0])
    geo.set_editor_property("origin_longitude", ORIGIN[1])
    geo.set_editor_property("origin_height", ORIGIN[2])

    sun = spawn(unreal.CesiumSunSky, "CesiumSunSky")
    try:
        sun.set_editor_property("time_zone", 8.0)
        sun.set_editor_property("solar_time", 15.0)  # daylight for photoreal tiles
    except Exception as e:
        unreal.log_warning("SunSky time props: {}".format(e))

    tiles = spawn(unreal.Cesium3DTileset, "CityTileset")
    if keys.get("cesium_ion_token") and not keys.get("prefer_osm_buildings"):
        # Google Photorealistic 3D Tiles served through Cesium ion (asset 2275207)
        tiles.set_editor_property("tileset_source", unreal.TilesetSource.FROM_CESIUM_ION)
        tiles.set_editor_property("ion_asset_id", 2275207)
        tiles.set_editor_property("ion_access_token", keys["cesium_ion_token"])
        unreal.log("CityTileset -> Google Photorealistic 3D Tiles (via ion)")
    elif keys.get("google_tiles_key"):
        tiles.set_editor_property("tileset_source", unreal.TilesetSource.FROM_URL)
        tiles.set_editor_property(
            "url",
            "https://tile.googleapis.com/v1/3dtiles/root.json?key="
            + keys["google_tiles_key"],
        )
        unreal.log("CityTileset -> Google Photorealistic 3D Tiles (direct)")
    elif keys.get("cesium_ion_token"):
        tiles.set_editor_property("tileset_source", unreal.TilesetSource.FROM_CESIUM_ION)
        tiles.set_editor_property("ion_asset_id", 96188)  # OSM Buildings
        tiles.set_editor_property("ion_access_token", keys["cesium_ion_token"])
        terrain = spawn(unreal.Cesium3DTileset, "Terrain")
        terrain.set_editor_property("tileset_source", unreal.TilesetSource.FROM_CESIUM_ION)
        terrain.set_editor_property("ion_asset_id", 1)  # World Terrain
        terrain.set_editor_property("ion_access_token", keys["cesium_ion_token"])
        unreal.log("CityTileset -> Cesium OSM Buildings + World Terrain")
    else:
        unreal.log_warning("No keys.json — tileset left unconfigured (add later in Cesium panel)")
    return geo


def spawn_beacons(geo):
    """Placeholder beacons: emissive pillar + label per site, globe-anchored.

    BP_AssetBeacon will replace these; the anchors/labels carry over.
    """
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    existing = {a.get_actor_label() for a in eas.get_all_level_actors()}
    cylinder = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Cylinder")

    with open(SITES_CSV) as f:
        rows = list(csv.DictReader(f))

    for row in rows:
        label = "Beacon_" + row["Name"]
        if label in existing:
            continue
        hero = row["Tier"] == "Hero"
        # georeference origin is fixed, so a static placement is enough:
        # convert lng/lat/height straight to level coordinates
        pos = geo.transform_longitude_latitude_height_position_to_unreal(
            unreal.Vector(float(row["Longitude"]), float(row["Latitude"]), 90.0))
        actor = eas.spawn_actor_from_class(unreal.StaticMeshActor, pos)
        actor.set_actor_label(label)
        actor.set_folder_path("Beacons")
        mesh_comp = actor.static_mesh_component
        mesh_comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
        mesh_comp.set_static_mesh(cylinder)
        h = 3.0 if hero else 1.6  # pillar height multiplier
        actor.set_actor_scale3d(unreal.Vector(0.6 if hero else 0.35,
                                              0.6 if hero else 0.35, h))
        unreal.log("beacon " + row["Org"])

    unreal.log("Beacons: {} sites processed".format(len(rows)))


def main():
    keys = load_keys()
    get_or_create_level()
    geo = setup_cesium(keys)
    spawn_beacons(geo)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.save_current_level()
    unreal.log("=== SingaporeTwin level saved ===")


main()
