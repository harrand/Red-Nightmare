rn.mods.basegame.buffs.enraged =
{
	name = "Enraged. Deal 50% more damage, ",
	duration = 8.0,
	internal = false,
	on_apply = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_physical_power(uuid, 0.5)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 3.0)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r, g * 0.5, b * 0.5)
	end,
	on_remove = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_physical_power(uuid, -0.5)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -3.0)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r, g * 2, b * 2)
	end,
}