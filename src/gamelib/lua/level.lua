rn = rn or {}
rn.level = rn.level or {}
rn.level.levels = rn.level.levels or {}

rn.level.load = function(name)
	rn.level.data_clear()
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

rn.level.data_read = function(...)
	local args = table.pack(...)
	local amended_args = {}
	for i,key in pairs(args) do
		if type(i) == 'number' then
			key = "current_level." .. key
			table.insert(amended_args, key)
		end
	end
	return rn.data_store():read_some(table.unpack(amended_args))
end

rn.level.data_write = function(...)
	-- args: current_level, {key1, value1, key2, value2}
	-- so if even, we're a key
	local args = {...}
	local amended_args = {}
	local counter=0
	for i,key_or_val in pairs(args) do
		if type(i) == 'number' then
			if counter % 2 == 0 then
				-- even. we're a key and we want to amend our name
				key_or_val = "current_level." .. key_or_val
			end
			counter = counter + 1
			table.insert(amended_args, key_or_val)
		end
	end
	rn.data_store():edit_some(table.unpack(amended_args))
end

rn.level.data_clear = function()
	tracy.ZoneBegin()
	rn.data_store():remove_all_of("current_level.")
	tracy.ZoneEnd()
end