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
			local dmg = rn.entity.prefabs.combat_stats.get_physical_power(uuid) * 10
			rn.entity.prefabs.combat_stats.dmg(target, dmg, "physical", uuid)
			rn.entity.prefabs.combat_stats.heal(uuid, dmg * 0.5, "shadow", uuid)
		end
	end,
	precast = function(me, target)
		-- instead of letting melee_ai deal with it, we're going to do an execute.
		rn.spell.cast(me, "zombie_devour")
		-- set `target` to be stunned
		rn.entity.stun(target, 10.0)
		-- make the victim face us.
		local mex, mey = rn.entity.prefabs.sprite.get_position(me)
		local vx, vy = rn.entity.prefabs.sprite.get_position(target)
		local dx = mex - vx
		local dy = mey - vy
		rn.entity.prefabs.bipedal.face_direction(target, dx, dy)
	end
}
