rn.mods.basegame.prefabs.light_emitter =
{
	description = "Entity gives off light for the duration of its lifetime.",
	instantiate = function(uuid)
		-- initial power of 69. hopefully noticeable enough that you think "why is it so bright oh i forgot to set power" you fuckin weapon
		local light = rn.renderer():add_light(0.0, 0.0, 0.0, 0.0, 0.0, 69.0)
		rn.current_scene():entity_write(uuid, "attached_light", light)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local light = sc:entity_read(uuid, "attached_light")
		local x, y, z = sc:entity_get_local_position(uuid)
		if light ~= nil then
			rn.renderer():light_set_position(light, x, y, z)
		end
	end,
	on_remove = function(uuid)
		local sc = rn.current_scene()
		local light = sc:entity_read(uuid, "attached_light")
		if light ~= nil then
			sc:entity_write(uuid, "attached_light", nil)
			rn.renderer():remove_light(light)
		end
	end,
	set_colour = function(uuid, r, g, b)
		local light = rn.current_scene():entity_read(uuid, "attached_light")
		if light ~= nil then
			rn.renderer():light_set_colour(light, r, g, b)
		end
	end,
	get_power = function(uuid)
		local light = rn.current_scene():entity_read(uuid, "attached_light")
		if light ~= nil then
			return rn.renderer():light_get_power(light)
		else
			return 0
		end
	end,
	set_power = function(uuid, pow)
		local light = rn.current_scene():entity_read(uuid, "attached_light")
		if light ~= nil then
			rn.renderer():light_set_power(light, pow)
		end
	end,
	set_shape = function(uuid, shape)
		local light = rn.current_scene():entity_read(uuid, "attached_light")
		if light ~= nil then
			rn.renderer():light_set_shape(light, shape)
		end
	end,
	exists = function(uuid)
		return rn.current_scene():entity_read(uuid, "attached_light") ~= nil
	end,
}