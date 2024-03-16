rn.mods.basegame.items.facial_hair_full_beard = 
{
	static_init = function()
		rn.renderer():add_texture("helm.facial_hair.full_beard", "basegame/res/skins/helms/facial_hair/full_beard.png")
	end,
	slot = rn.item.slot.helm,
	colour = {0.286, 0.184, 0},
	rarity = "common",
	texture = "helm.facial_hair.full_beard",
	valid_loot = false,
	droppable = false
}