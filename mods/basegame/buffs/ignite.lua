rn.mods.basegame.buffs.ignite =
{
	name = "Alight. Taking periodic fire damage.",
	duration = 4.0,
	on_advance = function(uuid, delta_seconds)
		-- stop ticking on things that are dead.
		if rn.entity.prefabs.combat_stats.is_dead(uuid) then
			rn.buff.remove(uuid, "ignite")
			return
		end
		local timer = rn.current_scene():entity_read(uuid, "ignite_timer") or 1.0		
		local tick_pct = 0.2
		local dmg = rn.current_scene():entity_read(uuid, "ignite_damage") or (1.0 / tick_pct)
		timer = timer - delta_seconds
		if timer < 0.0 then
			local causer = rn.current_scene():entity_read(uuid, "ignite_cause")
			if causer == nil or not rn.current_scene():contains_entity(causer) then
				causer = uuid
			end
			print("base dmg: " .. dmg .. ", tick: " .. dmg * tick_pct)
			rn.entity.prefabs.combat_stats.dmg(uuid, dmg * tick_pct, "fire", causer)
			timer = 1.0
		end
		rn.current_scene():entity_write(uuid, "ignite_timer", timer)
	end,
}