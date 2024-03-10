rn.mods.basegame.items.white_legion_platebody = 
{
	tooltip = [[
		+40% Physical Resist
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.4)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.4)
	end,
	slot = rn.item.slot.chest,
	colour = {0.8, 0.8, 0.8},
	rarity = "rare",
	texture = "chest.platebody"
}