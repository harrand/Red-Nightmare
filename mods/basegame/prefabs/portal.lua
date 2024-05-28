rn.mods.basegame.prefabs.portal =
{
	description = "A portal, where does it go?",
	static_init = function()
		rn.renderer():add_texture("sprite.portal", "basegame/res/sprites/portal.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.light_emitter.instantiate(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.portal")
		rn.entity.prefabs.sprite.set_scale(uuid, 1.75)
	end,
	on_remove = function(uuid)
		rn.entity.prefabs.light_emitter.on_remove(uuid)
	end,
	update = function(uuid, delta_seconds)
		local timer = rn.current_scene():entity_read(uuid, "timer") or 0.0
		timer = timer + delta_seconds
		rn.current_scene():entity_write(uuid, "timer", timer)
		rn.entity.prefabs.sprite.set_rotation(uuid, timer)
		rn.entity.prefabs.light_emitter.set_power(uuid, 0.5 * math.abs(math.sin(timer * 7.0)) + 1.5)
		rn.entity.prefabs.light_emitter.set_colour(uuid, rn.entity.prefabs.sprite.get_colour(uuid))
		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
		rn.entity.prefabs.light_emitter.set_shape(uuid, 1)
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
				-- take away player control and play an animation while we load into the next level.
				local currently_playing = rn.current_scene():entity_get_playing_animation(other)
				if currently_playing ~= "CastBlockStart" then
					rn.entity.prefabs.bipedal.play_animation(other, "CastBlockStart", false, 0.35)
				end
				rn.entity.prefabs.keyboard_controlled.set_enabled(other, false)
				local destination = rn.entity.prefabs.portal.get_level_destination(me)
				if destination ~= nil then
					rn.player.stash()
					rn.level.load(destination)
				else
					rn.player.stash()
					rn.level.reload()
				end
			else
				-- note: there's a couple of prefab types we dont want to remove.
				local prefab = rn.current_scene():entity_read(other, ".prefab")
				if prefab ~= "basic_target_field" then
					rn.current_scene():remove_entity(other)
				end
			end
		end
		return false
	end
}