rn = rn or {}
rn.level = rn.level or {}
rn.level.levels = rn.level.levels or {}

local fadeout_max = 1.0

rn.level.do_load = function(skip_fadein)
	local name = rn.level.data_read("next_level_name")
	local dont_stop_music = rn.level.data_read("next_level_dont_stop_music")
	local level = rn.level.levels[name]
	if level ~= nil then
		rn.level.data_clear()
		rn.current_scene():clear_entities()
		rn.renderer():clear_lights()
		rn.renderer():clear_strings()
		rn.renderer():set_ambient_light(1.0, 1.0, 1.0, 1.0)
		if dont_stop_music == nil then
			rn.stop_music(0)
		end
		if level.on_load ~= nil then
			level.on_load()
		end
		rn.level.data_write("name", name)
	else
		tz.report("Unknown level \"" .. name .. "\"")
		tz.assert(false);
	end
	rn.level.data_remove("fadeout")
	if not skip_fadein then
		rn.level.data_write("fadein", 0.0)
	end
	rn.player.unstash()
end

rn.level.load = function(name, dont_stop_music)
	rn.level.data_write("next_level_name", name)
	rn.level.data_write("next_level_dont_stop_music", dont_stop_music)
	local cur_level_name = rn.level.data_read("name")
	if cur_level_name == nil then
		rn.level.do_load(true)
	else
		local fadeout = rn.level.data_read("fadeout")
		if fadeout == nil then
			rn.level.data_write("fadeout", 0.0)
		end
	end
end

rn.level.reload = function(dont_stop_music)
	local name = rn.level.data_read("name")
	tz.assert(name ~= nil, "No level loaded!")
	rn.level.load(name, dont_stop_music)
end

rn.level.current_level_update = function(delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Current Level Update")

	local cur_level_name = rn.level.data_read("name")
	if cur_level_name == nil then
		return
	end

	local fadeout = rn.level.data_read("fadeout")
	if fadeout ~= nil then
		if fadeout > fadeout_max then
			rn.level.do_load()
			return
		else
			local grayscale_colour = (fadeout_max - fadeout) / fadeout_max
			rn.renderer():set_global_colour_multiplier(grayscale_colour, grayscale_colour, grayscale_colour)
			fadeout = fadeout + delta_seconds
			rn.level.data_write("fadeout", fadeout)
		end
	end

	local fadein = rn.level.data_read("fadein")
	if fadein ~= nil then
		if fadein > fadeout_max * 2.0 then
			rn.level.data_remove("fadein")
			rn.renderer():set_global_colour_multiplier(1.0, 1.0, 1.0)
		else
			-- instead of fading in from zero, we facein from 0.25 * fadeout_max
			-- otherwise on the 1st frame of the fadein everything is technically visible albeit dark.
			local grayscale_colour = fadein / fadeout_max
			if grayscale_colour > 1.0 then
				grayscale_colour = grayscale_colour - 1.0
				rn.renderer():set_global_colour_multiplier(grayscale_colour, grayscale_colour, grayscale_colour)
			end
			fadein = fadein + (delta_seconds * 2.0)
			rn.level.data_write("fadein", fadein)
		end
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

	if rn.input():is_key_down("esc") and rn.input():is_key_down("q") then
		rn.level.load("startscreen")
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

rn.level.data_remove = function(arg)
	rn.data_store():remove("current_level." .. arg)
end

rn.level.data_clear = function()
	tracy.ZoneBegin()
	rn.data_store():remove_all_of("current_level.")
	tracy.ZoneEnd()
end