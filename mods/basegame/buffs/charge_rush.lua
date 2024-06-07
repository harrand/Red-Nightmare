rn.mods.basegame.buffs.charge_rush =
{
	name = "Charging at a target location...",
	duration = nil,
	internal = true,
	on_apply = function(uuid)
		print("charge begin on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 10.0)
	end,
	on_remove = function(uuid)
		print("charge end on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -10.0)
		rn.current_scene():entity_write(uuid, "charge_stopped", false)
	end,
	on_advance = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		if sc:entity_read(uuid, "charge_stopped") == true then
			rn.buff.remove(uuid, "charge_rush")
			rn.spell.spells.savage_kick.finish(uuid)
			return
		end
		local dx = sc:entity_read(uuid, "charge_dirx")
		local dy = sc:entity_read(uuid, "charge_diry")
		if dx == nil or dy == nil then
			tz.error("did not get charge direction...")
			rn.buff.remove(uuid, "charge_rush")
		end
		rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
	end,
	on_collision = function(uuid_a, uuid_b, collision_responded)
		-- we could remove right now, however that would cause a bug.
		-- if we collide with 2 things at once, we will remove twice (and fuck up the movement speed permanently)
		-- what we do here is set some entity data to true instead. we can do that twice no worries. either way, next frame the condition is checked and removed once anyway.
		if collision_responded then
			rn.current_scene():entity_write(uuid_a, "charge_stopped", true)
		end
	end
}