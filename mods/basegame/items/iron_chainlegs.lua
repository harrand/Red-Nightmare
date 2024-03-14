rn.mods.basegame.items.iron_chainlegs =
{
	static_init = function()
		rn.renderer():add_texture("legs.chainlegs", "basegame/res/skins/legs/chainlegs.png")
	end,
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.15)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 3)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.15)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -3)
	end,
	slot = rn.item.slot.legs,
	colour = {0.5, 0.5, 0.5},
	texture = "legs.chainlegs",
}