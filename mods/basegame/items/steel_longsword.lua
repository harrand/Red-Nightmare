rn.mods.basegame.items.steel_longsword =
{
	tooltip = [[
		+3 Physical Power
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, 3.0)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -3.0)
	end,
	slot = rn.item.slot.right_hand,
	colour = {0.62, 0.6, 0.69},
	rarity = "rare",
	weapon_prefab = "weapon_model_sword"
}