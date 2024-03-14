rn.mods.basegame.items.holy_water_sprinkler =
{
	tooltip = [[
		+4 Physical Power
		+2 Holy Power
		+1.5 Movement Speed
		+20% more Holy Power
		+30% increased Haste
		(NYI: On Hit: Deal 5 additional damage as holy to undead targets.)
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, 4.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_holy_power(uuid, 2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 1.5)
		rn.entity.prefabs.combat_stats.apply_pct_more_holy_power(uuid, 1.2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -4.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_holy_power(uuid, -2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -1.5)
		rn.entity.prefabs.combat_stats.apply_pct_more_holy_power(uuid, 0.8333)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
	end,
	slot = rn.item.slot.right_hand,
	colour = 
	{
		rn.spell.schools.holy.colour[1] * 0.8,
		rn.spell.schools.holy.colour[2] * 0.8,
		rn.spell.schools.holy.colour[3] * 0.8
	},
	rarity = "epic",
	weapon_prefab = "weapon_morning_star",
	weapon_class = "mace"
}