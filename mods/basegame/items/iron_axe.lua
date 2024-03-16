rn.mods.basegame.items.iron_axe =
{
	tooltip = [[
		+3 Physical Power
		(NYI: On Hit: Deal 5 additional damage as holy to undead targets.)
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, 3.0)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -4.0)
	end,
	slot = rn.item.slot.right_hand,
	colour = {0.5, 0.5, 0.5},
	rarity = "common",
	weapon_prefab = "weapon_axe",
	weapon_class = "axe"
}