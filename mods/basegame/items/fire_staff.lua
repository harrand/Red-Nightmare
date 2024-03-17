rn.mods.basegame.items.fire_staff =
{
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, 5.0)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, -5.0)
	end,
	slot = rn.item.slot.left_hand,
	colour = {0.5, 0.25, 0.0},
	rarity = "rare",
	weapon_prefab = "weapon_model_staff",
	weapon_class = "staff"
}