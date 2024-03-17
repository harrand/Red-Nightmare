rn.mods.basegame.items.blazing_staff =
{
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, 9.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.2)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, -9.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.2)
	end,
	slot = rn.item.slot.left_hand,
	colour = rn.spell.schools.fire.colour,
	rarity = "epic",
	weapon_prefab = "weapon_model_staff",
	weapon_class = "staff"
}