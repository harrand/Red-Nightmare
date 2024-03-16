rn.mods.basegame.prefabs.portal =
{
	description = "A portal, where does it go?",
	static_init = function()
		rn.renderer():add_texture("sprite.portal", "basegame/res/sprites/portal.png")
		rn.renderer():add_texture("sprite.portal_normals", "basegame/res/sprites/portal_normals.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.light_emitter.instantiate(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.portal")
		rn.entity.prefabs.sprite.set_normal_map(uuid, "sprite.portal_normals")
		rn.entity.prefabs.sprite.set_rotation(uuid, -math.pi / 2.0)
		rn.entity.prefabs.sprite.set_scale(uuid, 1.75)
	end,
	on_remove = function(uuid)
		rn.entity.prefabs.light_emitter.on_remove(uuid)
	end,
	update = function(uuid, delta_seconds)
		local timer = rn.current_scene():entity_read(uuid, "timer") or 0.0
		timer = timer + delta_seconds
		rn.current_scene():entity_write(uuid, "timer", timer)
		rn.entity.prefabs.light_emitter.set_power(uuid, 0.5 * math.abs(math.sin(timer * 7.0)) + 1.5)
		rn.entity.prefabs.light_emitter.set_colour(uuid, rn.entity.prefabs.sprite.get_colour(uuid))
		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
	end,
	set_colour = function(uuid, r, g, b)
		rn.entity.prefabs.sprite.set_colour(uuid, r, g, b)
	end,
	set_level_destination = function(uuid, level_name)
		rn.current_scene():entity_write(uuid, "destination", level_name)
	end,
	get_level_destination = function(uuid)
		return rn.current_scene():entity_read(uuid, "destination")
	end,
	on_collision = function(me, other)
		if other ~= nil and rn.current_scene():contains_entity(other) then
			if rn.level.data_read("player") == other then
				rn.player.stash()
				local difficulty = rn.data_store():read("difficulty") or 0
				rn.data_store():set("difficulty", difficulty + 1)
				local destination = rn.entity.prefabs.portal.get_level_destination(me)
				if destination ~= nil then
					rn.level.load(destination)
				else
					rn.level.reload()
				end
				rn.player.unstash()
			else
				rn.current_scene():remove_entity(other)
			end
		end
		return false
	end
}