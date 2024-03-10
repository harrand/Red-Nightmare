rn.mods.basegame.items.small_iron_shield =
{
	tooltip = [[
		+ 40% Physical Resist
		+ 30% Frost Resist
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.4)
		rn.entity.prefabs.combat_stats.apply_flat_increased_frost_resist(uuid, 0.3)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.4)
		rn.entity.prefabs.combat_stats.apply_flat_increased_frost_resist(uuid, -0.3)
	end,
	slot = rn.item.slot.left_hand,
	colour = {0.5, 0.5, 0.5},
	rarity = "common",
	weapon_prefab = "weapon_small_shield"
}