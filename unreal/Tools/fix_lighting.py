import unreal
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.load_level("/Game/Maps/SingaporeTwin")
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

# replace scripted CesiumSunSky with a deterministic standard lighting stack
for a in list(eas.get_all_level_actors()):
    if a.get_class().get_name().startswith("CesiumSunSky"):
        eas.destroy_actor(a)
        unreal.log("removed CesiumSunSky")

labels = {a.get_actor_label() for a in eas.get_all_level_actors()}
def spawn(cls, label, rot=unreal.Rotator(0,0,0)):
    if label in labels: return None
    a = eas.spawn_actor_from_class(cls, unreal.Vector(0,0,0), rot)
    a.set_actor_label(label)
    unreal.log("spawned " + label)
    return a

sun = spawn(unreal.DirectionalLight, "Sun", unreal.Rotator(0.0, -50.0, 30.0))
if sun:
    c = sun.get_component_by_class(unreal.DirectionalLightComponent)
    c.set_editor_property("intensity", 8.0)          # lux-style for atmosphere
    c.set_editor_property("atmosphere_sun_light", True)
spawn(unreal.SkyAtmosphere, "Atmosphere")
sky = spawn(unreal.SkyLight, "Ambient")
if sky:
    c = sky.get_component_by_class(unreal.SkyLightComponent)
    c.set_editor_property("real_time_capture", True)

# sanity: where are the player start and a beacon?
for a in eas.get_all_level_actors():
    if a.get_actor_label() in ("IslandStart", "Asset_changi", "Asset_dbs"):
        unreal.log("{} at {}".format(a.get_actor_label(), a.get_actor_location()))
les.save_current_level()
unreal.log("lighting rebuilt + saved")
