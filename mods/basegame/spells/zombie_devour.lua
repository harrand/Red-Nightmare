rn.mods.basegame.spells.zombie_devour =
{
	cast_duration = 2.0,
	magic_type = "physical",
	animation_override = "ZombieExecute",
	description = "Bites into the flesh of the target, dealing massive physical damage. The target is stunned for the duration.",
	finish = function(uuid, casterx, castery)
		local target = rn.current_scene():entity_read(uuid, "devour_victim")
		if target ~= nil and rn.current_scene():contains_entity(target) then
			rn.entity.unstun(target)
			local dmg = 10
			rn.entity.prefabs.combat_stats.dmg(target, dmg, "physical", uuid)
			rn.entity.prefabs.combat_stats.heal(uuid, dmg * 0.5, "shadow", uuid)
			rn.current_scene():entity_write(uuid, "devour_victim", nil)
			rn.current_scene():entity_write(uuid, "zombie_devour_last_cast", tz.time())
		end
	end,
	advance = function(uuid, cast_progress)
		local victim = rn.current_scene():entity_read(uuid, "devour_victim")
		-- if the victim despawned or died since we started devouring, then cancel this cast.
		if victim == nil or not rn.current_scene():contains_entity(victim) or rn.entity.prefabs.combat_stats.is_dead(victim) then
			rn.spell.clear(uuid)
			rn.current_scene():entity_write(uuid, "devour_victim", nil)
		end
	end,
	on_cast_begin = function(me)
		local target = rn.entity.prefabs.base_ai.get_target(me)
		tz.assert(target ~= nil and rn.current_scene():contains_entity(target), "Attmept to precast zombie_devour on an entity that doesn't exist.")
		-- set `target` to be stunned
		rn.entity.stun(target, 10.0)
		rn.current_scene():entity_write(me, "devour_victim", target)
		-- make the victim face us.
		local mex, mey = rn.entity.prefabs.sprite.get_position(me)
		local vx, vy = rn.entity.prefabs.sprite.get_position(target)
		local dx = vx - mex
		local dy = vy - mey
		rn.entity.prefabs.bipedal.face_direction(target, dx, dy)
	end,
	is_on_cooldown = function(uuid)
		local casted_at = rn.current_scene():entity_read(uuid, "zombie_devour_last_cast") or 0.0
		return tz.time() < (casted_at + 5000)
	end
}
