import unreal
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.load_level("/Game/Maps/SingaporeTwin")
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == "CesiumSunSky":
        a.set_editor_property("solar_time", 15.0)  # mid-afternoon: photoreal tiles need daylight
        unreal.log("SunSky -> 15:00")
les.save_current_level()
unreal.log("saved")
