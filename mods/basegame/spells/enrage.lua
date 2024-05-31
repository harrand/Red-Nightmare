rn.mods.basegame.spells.enrage =
{
	cast_duration = 0.0,
	magic_type = "physical",
	slot = "red",
	dont_face_direction = true,
	description = "Enter a berserker rage, increasing your haste and physical damage dealt.",
	finish = function(uuid, casterx, castery)
		if rn.buff.is_applied(uuid, "enraged") then return end
		rn.buff.apply(uuid, "enraged")
	end
}