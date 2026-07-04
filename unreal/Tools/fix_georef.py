import unreal
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
les.load_level("/Game/Maps/SingaporeTwin")
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for a in eas.get_all_level_actors():
    if a.get_class().get_name() == "CesiumGeoreference":
        for p in ("origin_latitude", "origin_longitude", "origin_height"):
            try: unreal.log("{} = {}".format(p, a.get_editor_property(p)))
            except Exception as e: unreal.log_warning(str(e))
        try:
            a.set_origin_longitude_latitude_height(unreal.Vector(103.8200, 1.3400, 250.0))
            unreal.log("origin set via method")
        except Exception as e:
            unreal.log_warning("method failed: {}".format(e))
            a.set_editor_property("origin_latitude", 1.34)
            a.set_editor_property("origin_longitude", 103.82)
            a.set_editor_property("origin_height", 250.0)
        for p in ("origin_latitude", "origin_longitude"):
            unreal.log("after: {} = {}".format(p, a.get_editor_property(p)))
les.save_current_level()
unreal.log("georef saved")
