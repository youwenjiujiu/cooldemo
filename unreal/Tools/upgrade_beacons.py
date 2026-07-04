"""
Phase 2: glow material + real AAssetBeacon actors + player start.

Run after the IslandGrid C++ module is compiled:
  UnrealEditor-Cmd unreal/IslandGrid.uproject \
    -ExecutePythonScript=unreal/Tools/upgrade_beacons.py -unattended -nosplash
"""
import csv
import os

import unreal

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LEVEL_PATH = "/Game/Maps/SingaporeTwin"
SITES_CSV = os.path.join(ROOT, "Content", "Data", "Sites.csv")
MAT_DIR = "/Game/Materials"
MAT_PATH = MAT_DIR + "/M_Beacon"

CYAN = unreal.LinearColor(0.10, 3.2, 2.9, 1.0)
AMBER = unreal.LinearColor(4.0, 1.6, 0.25, 1.0)


def ensure_material():
    if unreal.EditorAssetLibrary.does_asset_exist(MAT_PATH):
        unreal.log("M_Beacon exists")
        return
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat = tools.create_asset("M_Beacon", MAT_DIR, unreal.Material,
                             unreal.MaterialFactoryNew())
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mel = unreal.MaterialEditingLibrary
    param = mel.create_material_expression(
        mat, unreal.MaterialExpressionVectorParameter, -420, 0)
    param.set_editor_property("parameter_name", "Emissive")
    param.set_editor_property("default_value", CYAN)
    mel.connect_material_property(param, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(MAT_PATH)
    unreal.log("Created M_Beacon (unlit emissive)")


def rebuild_beacons():
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.load_level(LEVEL_PATH)
    eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    geo = None
    for a in eas.get_all_level_actors():
        if a.get_class().get_name() == "CesiumGeoreference":
            geo = a
            break
    if not geo:
        raise RuntimeError("CesiumGeoreference not found")

    # drop placeholder pillars
    removed = 0
    for a in list(eas.get_all_level_actors()):
        if a.get_actor_label().startswith("Beacon_") and \
                a.get_class().get_name() == "StaticMeshActor":
            eas.destroy_actor(a)
            removed += 1
    unreal.log("Removed {} placeholder pillars".format(removed))

    beacon_cls = unreal.load_class(None, "/Script/IslandGrid.AssetBeacon")
    existing = {a.get_actor_label() for a in eas.get_all_level_actors()}

    with open(SITES_CSV) as f:
        rows = list(csv.DictReader(f))
    for row in rows:
        label = "Asset_" + row["Name"]
        if label in existing:
            continue
        pos = geo.transform_longitude_latitude_height_position_to_unreal(
            unreal.Vector(float(row["Longitude"]), float(row["Latitude"]), 15.0))
        actor = eas.spawn_actor_from_class(beacon_cls, pos)
        actor.set_actor_label(label)
        actor.set_folder_path("Assets")
        actor.set_editor_property("site_key", row["Name"])
        actor.set_editor_property("org", row["Org"])
        actor.set_editor_property(
            "tier", unreal.BeaconTier.HERO if row["Tier"] == "Hero"
            else unreal.BeaconTier.MOCK)
        actor.set_editor_property(
            "glow_color", AMBER if row["LightTone"] == "Amber" else CYAN)
    unreal.log("Spawned {} AssetBeacons".format(len(rows)))

    # player start: high above Marina Bay looking across the island
    if "IslandStart" not in existing:
        start_pos = geo.transform_longitude_latitude_height_position_to_unreal(
            unreal.Vector(103.8600, 1.2400, 2600.0))
        # ESU frame: X=East, Y=South -> facing north is yaw -90
        start = eas.spawn_actor_from_class(unreal.PlayerStart, start_pos,
                                           unreal.Rotator(0.0, 0.0, -90.0))
        start.set_actor_label("IslandStart")
        unreal.log("PlayerStart placed")

    les.save_current_level()
    unreal.log("=== beacons upgraded, level saved ===")


ensure_material()
rebuild_beacons()
