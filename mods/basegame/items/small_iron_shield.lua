rn.mods.basegame.items.small_iron_shield =
{
	tooltip = [[
		+ 10% Magic Resist
		+ 30% Physical Resist
	]],
	on_equip = function(uuid)
		for schoolname, schooldata in pairs(rn.spell.schools) do
			rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, 0.1)
		end
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.2)
	end,
	on_unequip = function(uuid)
		for schoolname, schooldata in pairs(rn.spell.schools) do
			rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, -0.1)
		end
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.2)
	end,
	slot = rn.item.slot.left_hand,
	colour = {0.5, 0.5, 0.5},
	rarity = "common",
	weapon_prefab = "weapon_small_shield"
}