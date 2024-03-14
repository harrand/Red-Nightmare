rn.mods.basegame.items.iron_chainmail =
{
	static_init = function()
		rn.renderer():add_texture("chest.chainmail", "basegame/res/skins/chests/chainmail.png")
	end,
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.25)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 5)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.25)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -5)
	end,
	slot = rn.item.slot.chest,
	colour = {0.5, 0.5, 0.5},
	texture = "chest.chainmail",
}