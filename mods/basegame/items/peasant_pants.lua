rn.mods.basegame.items.peasant_pants =
{
	static_init = function()
		rn.renderer():add_texture("legs.pants", "basegame/res/skins/legs/pants.png")
	end,
	slot = rn.item.slot.legs,
	colour = {0.286, 0.184, 0.0},
	rarity = "common",
	valid_loot = false,
	droppable = false,
	texture = "legs.pants"
}