rn.mods.basegame.buffs.enraged =
{
	name = "Enraged. Deal 50% more damage, ",
	duration = 8.0,
	on_apply = function(uuid)
		print("charge begin on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_physical_power(uuid, 0.5)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r, g * 0.5, b * 0.5)
	end,
	on_remove = function(uuid)
		print("charge end on entity " .. uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
		rn.entity.prefabs.combat_stats.apply_pct_increased_physical_power(uuid, -0.5)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r, g * 2, b * 2)
	end,
}