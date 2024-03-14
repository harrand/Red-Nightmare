rn.mods.basegame.items.iron_sallet =
{
	static_init = function()
		rn.renderer():add_texture("helm.med_helm", "basegame/res/skins/helms/med_helm.png")
	end,
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.1)
		rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, 0.2)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.1)
		rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, -0.2)
	end,
	slot = rn.item.slot.helm,
	colour = {0.5, 0.5, 0.5},
	texture = "helm.med_helm",
}