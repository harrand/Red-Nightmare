rn.mods.basegame.items.peasant_shirt =
{
	static_init = function()
		rn.renderer():add_texture("chest.shirt", "basegame/res/skins/chests/shirt.png")
	end,
	slot = rn.item.slot.chest,
	colour = {0.46, 0.48, 0.54},
	rarity = "common",
	valid_loot = false,
	droppable = false,
	texture = "chest.shirt",
}