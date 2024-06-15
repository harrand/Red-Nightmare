for schoolname, schooldata in pairs(rn.spell.schools) do
	if schoolname ~= "physical" then
		rn.mods.basegame.items[schoolname .. "_elemental_hood"] =
		{
			static_init = function()
				if schoolname == "fire" then
					rn.renderer():add_texture("helm.hood", "basegame/res/skins/helms/hood.png")
					rn.renderer():add_texture("helm.hood_normals", "basegame/res/skins/helms/hood_normals.png")
					rn.renderer():add_texture("helm.hood_emissive", "basegame/res/skins/helms/hood_emissive.png")
				end
			end,
			on_equip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.02)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 2)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, 0.1)
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.02)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -2)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, -0.1)
			end,
			slot = rn.item.slot.helm,
			colour = schooldata.colour,
			texture = "helm.hood",
			normal_map = "helm.hood_normals",
			emissive_map = "helm.hood_emissive",
			emissive_tint = schooldata.colour,
			valid_loot = false,
			droppable = false,
			rarity = "common"
		}

		rn.mods.basegame.items[schoolname .. "_elemental_robe"] =
		{
			static_init = function()
				if schoolname == "fire" then
					rn.renderer():add_texture("chest.elemental_robes", "basegame/res/skins/chests/elemental_robes.png")
					rn.renderer():add_texture("chest.elemental_robes_emissive", "basegame/res/skins/chests/elemental_robes_emissive.png")
				end
			end,
			on_equip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.04)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 3)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, 0.2)
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.04)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -3)
				rn.entity.prefabs.combat_stats["apply_pct_increased_" .. schoolname .. "_power"](uuid, -0.2)
			end,
			slot = rn.item.slot.chest,
			colour = schooldata.colour,
			emissive_map = "chest.elemental_robes_emissive",
			emissive_tint = schooldata.colour,
			texture = "chest.elemental_robes",
			valid_loot = false,
			droppable = false,
			rarity = "common"
		}
	end
end