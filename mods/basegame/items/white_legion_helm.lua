rn.mods.basegame.items.white_legion_helm = 
{
	tooltip = [[
		+20% Physical Resist
		+2 Health per 5 seconds
	]],
	static_init = function()
		rn.renderer():add_texture("helm.full_helm", "basegame/res/skins/helms/full_helm.png")
	end,
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 3)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -3)
	end,
	on_update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "white_legion_helm_timer") or 0.0
		t = t + delta_seconds

		if t > 5.0 then
			rn.entity.prefabs.combat_stats.heal(uuid, 2, "holy", uuid)
			t = 0.0
		end

		sc:entity_write(uuid, "white_legion_helm_timer", t)
	end,
	slot = rn.item.slot.helm,
	colour = {0.8, 0.8, 0.8},
	rarity = "rare",
	texture = "helm.full_helm"
}