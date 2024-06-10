rn.mods.basegame.spells.sacred_shield =
{
	cast_duration = 1.8,
	magic_type = "holy",
	cast_type = "omni",
	slot = "yellow",
	icon = "icon.holybolt",
	cooldown = 20.0,
	description = "Conjures a magical protective barrier on the caster, absorbing damage.",
	finish = function(uuid, casterx, castery)
		rn.buff.apply(uuid, "sacred_shield_absorb")
	end
}