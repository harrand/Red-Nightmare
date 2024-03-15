rn.mods.basegame.items.imperial_plate_platebody = 
{
	tooltip = [[
		+20% Physical Resist
		+15 Max Health
		+25% Elemental Resist
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 15)
		for schoolname, schooldata in pairs(rn.spell.schools) do
			if schoolname ~= "physical" then
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, 0.25)
			end
		end
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.2)
		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -15)
		for schoolname, schooldata in pairs(rn.spell.schools) do
			if schoolname ~= "physical" then
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, -0.25)
			end
		end
	end,
	slot = rn.item.slot.chest,
	colour = {0.6, 0.2, 0.2},
	rarity = "epic",
	texture = "chest.platebody"
}