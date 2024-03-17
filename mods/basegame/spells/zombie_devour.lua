rn.mods.basegame.spells.zombie_devour =
{
	cast_duration = 2.0,
	magic_type = "physical",
	animation_override = "ZombieExecute",
	description = "Bites into the flesh of the target, dealing massive physical damage. The target is stunned for the duration.",
	finish = function(uuid, casterx, castery)
		local target = rn.current_scene():entity_read(uuid, "target")
		if target ~= nil and rn.current_scene():contains_entity(target) then
			rn.entity.unstun(target)
			local dmg = rn.entity.prefabs.combat_stats.get_base_physical_power(uuid) * 10
			rn.entity.prefabs.combat_stats.dmg(target, dmg, "physical", uuid)
			rn.entity.prefabs.combat_stats.heal(uuid, dmg * 0.5, "shadow", uuid)
		end
	end,
	on_cast_begin = function(me)
		local target = rn.entity.prefabs.base_ai.get_target(me)
		tz.assert(target ~= nil and rn.current_scene():contains_entity(target), "Attmept to precast zombie_devour on an entity that doesn't exist.")
		-- set `target` to be stunned
		rn.entity.stun(target, 10.0)
		-- make the victim face us.
		local mex, mey = rn.entity.prefabs.sprite.get_position(me)
		local vx, vy = rn.entity.prefabs.sprite.get_position(target)
		local dx = vx - mex
		local dy = vy - mey
		rn.entity.prefabs.bipedal.face_direction(target, dx, dy)
	end
}
