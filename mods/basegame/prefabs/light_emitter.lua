rn.mods.basegame.prefabs.light_emitter =
{
	description = "Entity gives off light for the duration of its lifetime.",
	instantiate = function(uuid)
		local light = rn.renderer():add_light(0.0, 0.0, 0.0, 0.0, 0.0, 2.0)
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
	exists = function(uuid)
		return rn.current_scene():entity_read(uuid, "attached_light") ~= nil
	end,
}