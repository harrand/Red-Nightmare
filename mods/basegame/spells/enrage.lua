rn.mods.basegame.spells.enrage =
{
	static_init = function()
		rn.renderer():add_texture("icon.enrage", "basegame/res/icons/enrage.png")
	end,
	cast_duration = 0.0,
	magic_type = "physical",
	slot = "red",
	dont_face_direction = true,
	icon = "icon.enrage",
	cooldown = 60.0,
	description = "Enter a berserker rage, increasing your haste and physical damage dealt.",
	finish = function(uuid, casterx, castery)
		if rn.buff.is_applied(uuid, "enraged") then return end
		rn.buff.apply(uuid, "enraged")
	end
}