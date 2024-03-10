rn.mods.basegame.items.white_legion_platelegs =
{
	static_init = function()
		rn.renderer():add_texture("legs.platelegs", "basegame/res/skins/legs/platelegs.png")
	end,
	tooltip = [[

	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.25)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.25)
	end,
	slot = rn.item.slot.legs,
	colour = {0.8, 0.8, 0.8},
	rarity = "rare",
	texture = "legs.platelegs"

}