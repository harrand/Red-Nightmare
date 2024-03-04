rn = rn or {}
rn.level = rn.level or {}
rn.level.levels = rn.level.levels or {}

rn.level.load = function(name)
	local level = rn.level.levels[name]
	if level ~= nil then
		rn.current_scene():clear_entities()
		rn.renderer():clear_lights()
		rn.renderer():clear_strings()
		rn.stop_music(0)
		if level.on_load ~= nil then
			level.on_load()
		end
		rn.current_scene():set_level_name(name)
	else
		tz.report("Unknown level \"" .. name .. "\"")
		tz.assert(false);
	end
end

rn.level.current_level_update = function(delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Current Level Update")

	local cur_level_name = rn.current_scene():get_level_name()
	if cur_level_name == nil then
		return
	end

	local level = rn.level.levels[cur_level_name]
	if level == nil then
		tz.report("Unknown level \"" .. cur_level_name .. "\"")
		tz.assert(false)
		return
	end
	if level.update ~= nil then
		level.update(delta_seconds)
	end
end