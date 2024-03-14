rn.mods.basegame.items.basic_torch =
{
	tooltip = [[
		Equipped: Emits a small amount of light.
	]],

	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, 2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_frost_resist(uuid, 0.1)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_movement_speed(uuid, -2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_frost_resist(uuid, -0.1)
	end,
	slot = rn.item.slot.left_hand,
	colour = {0.5, 0.25, 0.0},
	rarity = "common",
	weapon_prefab = "weapon_model_torch",
	weapon_class = "torch"
}