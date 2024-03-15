rn.mods.basegame.items.imperial_plate_platelegs =
{
	tooltip = [[
		+12.5% Physical Resist
		+10 Max Health
		+1 Movement Speed
		+10% Haste
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.125)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 10)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 1)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.1)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.125)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -10)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -1)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.1)
	end,
	slot = rn.item.slot.legs,
	colour = {0.6, 0.2, 0.2},
	rarity = "epic",
	texture = "legs.platelegs"
}