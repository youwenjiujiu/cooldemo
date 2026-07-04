import unreal
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.load_level("/Game/Maps/SingaporeTwin")
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in eas.get_all_level_actors():
    if a.get_class().get_name().startswith("CesiumSunSky"):
        unreal.log("class: " + a.get_class().get_name())
        for prop in ("solar_time", "time_zone", "elevation"):
            try:
                unreal.log("{} = {}".format(prop, a.get_editor_property(prop)))
            except Exception as e:
                unreal.log_warning("{}: {}".format(prop, e))
        try:
            a.set_editor_property("solar_time", 15.0)
            a.call_method("UpdateSun")
            unreal.log("UpdateSun called")
        except Exception as e:
            unreal.log_warning("UpdateSun failed: {}".format(e))
        # inspect the embedded directional light
        for c in a.get_components_by_class(unreal.DirectionalLightComponent):
            unreal.log("sun dir rot = {}  intensity = {}".format(
                c.get_world_rotation(), c.get_editor_property("intensity")))
les.save_current_level()
unreal.log("saved")
