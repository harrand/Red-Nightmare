rn.mods.basegame.buffs.chilled =
{
	name = "Chilled. Movement speed and haste reduced.",
	duration = 4.0,
	on_apply = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, -0.4)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r * 0.5, g * 0.5, b)
	end,
	on_remove = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, 0.4)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r * 2, g * 2, b)
	end,
}