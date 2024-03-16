rn.mods.basegame.items.facial_hair_walrus = 
{
	static_init = function()
		rn.renderer():add_texture("helm.facial_hair.walrus", "basegame/res/skins/helms/facial_hair/walrus.png")
	end,
	slot = rn.item.slot.helm,
	colour = {0.286, 0.184, 0},
	rarity = "common",
	texture = "helm.facial_hair.walrus",
	valid_loot = false,
	droppable = false
}