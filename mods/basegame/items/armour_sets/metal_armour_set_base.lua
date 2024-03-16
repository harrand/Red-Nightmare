function rn_impl_create_metal_armour_set(metal_name, metal_colour, metal_rarity, item_level)
	local items = rn.mods.basegame.items

	-- physical resist per item level (incl. coeff)
	local physical_resist_factor = 0.02
	local physical_power_factor = 1.5
	local max_hp_factor = 0.95
	local movement_speed_factor = 0.05
	local shield_resist_all_factor = 0.025

	local coif_coeff = item_level * 0.6
	items[metal_name .. "_coif"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * coif_coeff)
			rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, movement_speed_factor * coif_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * coif_coeff)
			rn.entity.prefabs.combat_stats.apply_pct_increased_movement_speed(uuid, -movement_speed_factor * coif_coeff)
		end,
		slot = rn.item.slot.helm,
		colour = metal_colour,
		texture = "helm.chain_helm",
		rarity = metal_rarity
	}

	local sallet_coeff = item_level * 0.65
	items[metal_name .. "_sallet"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * sallet_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * sallet_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * sallet_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * sallet_coeff)
		end,
		slot = rn.item.slot.helm,
		colour = metal_colour,
		texture = "helm.med_helm",
		rarity = metal_rarity
	}

	local helm_coeff = item_level * 0.7
	items[metal_name .. "_helm"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * helm_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * helm_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * helm_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * helm_coeff)
		end,
		slot = rn.item.slot.helm,
		colour = metal_colour,
		texture = "helm.full_helm",
		rarity = metal_rarity
	}

	local chainmail_coeff = item_level * 0.75
	items[metal_name .. "_chainmail"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * chainmail_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * chainmail_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * chainmail_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * chainmail_coeff)
		end,
		slot = rn.item.slot.chest,
		colour = metal_colour,
		texture = "chest.chainmail",
		rarity = metal_rarity
	}

	local breastplate_coeff = item_level * 0.85
	items[metal_name .. "_breastplate"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * breastplate_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * breastplate_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * breastplate_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * breastplate_coeff)
		end,
		slot = rn.item.slot.chest,
		colour = metal_colour,
		texture = "chest.breastplate",
		rarity = metal_rarity
	}

	local platebody_coeff = item_level * 1.0
	items[metal_name .. "_platebody"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * platebody_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * platebody_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * platebody_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * platebody_coeff)
		end,
		slot = rn.item.slot.chest,
		colour = metal_colour,
		texture = "chest.platebody",
		rarity = metal_rarity
	}

	local chainlegs_coeff = item_level * 0.7
	items[metal_name .. "_chainlegs"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * chainlegs_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * chainlegs_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * chainlegs_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * chainlegs_coeff)
		end,
		slot = rn.item.slot.legs,
		colour = metal_colour,
		texture = "legs.chainlegs",
		rarity = metal_rarity
	}

	local platelegs_coeff = item_level * 0.8
	items[metal_name .. "_platelegs"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * platelegs_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * platelegs_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * platelegs_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * platelegs_coeff)
		end,
		slot = rn.item.slot.legs,
		colour = metal_colour,
		texture = "legs.platelegs",
		rarity = metal_rarity
	}

	local shield_coeff = item_level * 0.4
	items[metal_name .. "_small_shield"] =
	{
		on_equip = function(uuid)
			for schoolname, schooldata in pairs(rn.spell.schools) do
				rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](uuid, shield_resist_all_factor * shield_coeff)
			end
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, physical_resist_factor * shield_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, max_hp_factor * shield_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -physical_resist_factor * shield_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -max_hp_factor * shield_coeff)
		end,
		slot = rn.item.slot.left_hand,
		colour = metal_colour,
		weapon_prefab = "weapon_small_shield",
		weapon_class = "shield",
		rarity = metal_rarity
	}

	local mace_coeff = item_level * 0.6
	items[metal_name .. "_mace"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, physical_power_factor * mace_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -physical_power_factor * mace_coeff)
		end,
		slot = rn.item.slot.right_hand,
		colour = metal_colour,
		weapon_prefab = "weapon_model_morning_star",
		weapon_class = "mace",
		rarity = metal_rarity
	}

	local sword_coeff = item_level * 0.6
	items[metal_name .. "_sword"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, physical_power_factor * sword_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -physical_power_factor * sword_coeff)
		end,
		slot = rn.item.slot.right_hand,
		colour = metal_colour,
		weapon_prefab = "weapon_model_sword",
		weapon_class = "sword",
		rarity = metal_rarity
	}

	local axe_coeff = item_level * 0.6
	items[metal_name .. "_axe"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, physical_power_factor * axe_coeff)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -physical_power_factor * axe_coeff)
		end,
		slot = rn.item.slot.right_hand,
		colour = metal_colour,
		weapon_prefab = "weapon_model_axe",
		weapon_class = "axe",
		rarity = metal_rarity
	}

	local sword2h_coeff = item_level * 1.1
	items[metal_name .. "_greatsword"] =
	{
		on_equip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, physical_power_factor * sword2h_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
		end,
		on_unequip = function(uuid)
			rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -physical_power_factor * sword2h_coeff)
			rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
		end,
		slot = rn.item.slot.right_hand,
		two_handed = true,
		colour = metal_colour,
		weapon_prefab = "weapon_model_sword2h",
		weapon_class = "sword2h",
		rarity = metal_rarity
	}
end


function rn_impl_static_armour_load()
	rn.renderer():add_texture("helm.chain_helm", "basegame/res/skins/helms/chain_helm.png")
	rn.renderer():add_texture("helm.med_helm", "basegame/res/skins/helms/med_helm.png")
	rn.renderer():add_texture("helm.full_helm", "basegame/res/skins/helms/full_helm.png")
	rn.renderer():add_texture("chest.chainmail", "basegame/res/skins/chests/chainmail.png")
	rn.renderer():add_texture("chest.breastplate", "basegame/res/skins/chests/breastplate.png")
	rn.renderer():add_texture("chest.platebody", "basegame/res/skins/chests/platebody.png")
	rn.renderer():add_texture("legs.chainlegs", "basegame/res/skins/legs/chainlegs.png")
	rn.renderer():add_texture("legs.platelegs", "basegame/res/skins/legs/platelegs.png")
end