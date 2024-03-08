rn.mods.basegame.spells.summon_zombie =
{
	cast_duration = 1.25,
	magic_type = "shadow",
	two_handed = false,
	cast_type = "omni",
	description = "Summons a zombie (at the origin of the world)",
	finish = function(uuid, casterx, castery)
		rn.current_scene():add_entity("zombie")
	end
}
