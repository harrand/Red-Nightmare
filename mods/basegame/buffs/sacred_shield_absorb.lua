rn.mods.basegame.buffs.sacred_shield_absorb =
{
	name = "Protected by a holy barrier",
	duration = 12.0,
	internal = false,
	on_apply = function(uuid)
		local barrier = rn.current_scene():add_entity("magic_barrier")
		rn.current_scene():entity_write(barrier, "magic_type", "holy")
		rn.entity.prefabs.sticky.stick_to(barrier, uuid, 0.0, 1.75, 0.0)
		rn.current_scene():entity_write(uuid, "magic_barrier_effect", barrier)
	end,
	on_remove = function(uuid)
		local barrier = rn.current_scene():entity_read(uuid, "magic_barrier_effect")
		if barrier ~= nil and rn.current_scene():contains_entity(barrier) then
			rn.current_scene():remove_entity(barrier)
		end
	end,
	on_advance = function(uuid, delta_seconds)
		-- stop ticking on things that are dead.
		if rn.entity.prefabs.combat_stats.is_dead(uuid) then
			rn.buff.remove(uuid, "sacred_shield_absorb")
			return
		end
		local timer = rn.current_scene():entity_read(uuid, "sacred_timer") or 1.0		
		local tick_pct = 0.2
		timer = timer - delta_seconds
		if timer < 0.0 then
			rn.entity.prefabs.combat_stats.heal(uuid, 1, "holy", uuid)
			timer = 1.0
		end
		rn.current_scene():entity_write(uuid, "sacred_timer", timer)
	end,
}