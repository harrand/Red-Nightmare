rn.mods.basegame.items.imperial_plate_helm = 
{
	tooltip = [[
		+20% Physical Resist
		+1 Health per second
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.15)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 8)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.15)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -8)
	end,
	on_update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "imperial_plate_helm_timer") or 0.0
		t = t + delta_seconds

		if t > 1.0 then
			rn.entity.prefabs.combat_stats.heal(uuid, 1, "shadow", uuid)
			t = 0.0
		end

		sc:entity_write(uuid, "imperial_plate_helm_timer", t)
	end,
	slot = rn.item.slot.helm,
	colour = {0.6, 0.2, 0.2},
	rarity = "epic",
	texture = "helm.full_helm"
}