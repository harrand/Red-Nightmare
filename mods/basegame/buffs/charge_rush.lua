rn.mods.basegame.buffs.charge_rush =
{
	name = "Charging at a target location...",
	duration = nil,
	on_apply = function(uuid)
		tz.report("charge begin on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 10.0)
	end,
	on_remove = function(uuid)
		tz.report("charge end on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -10.0)
	end,
	on_advance = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local dx = sc:entity_read(uuid, "charge_dirx")
		local dy = sc:entity_read(uuid, "charge_diry")
		if dx == nil or dy == nil then
			tz.report("did not get charge direction...")
			rn.buff.remove(uuid_a, "charge_rush")
		end
		rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
	end,
	on_collision = function(uuid_a, uuid_b, collision_responded)
		if collision_responded then
			rn.buff.remove(uuid_a, "charge_rush")
			rn.spell.spells.savage_kick.finish(uuid_a)
		end
	end
}