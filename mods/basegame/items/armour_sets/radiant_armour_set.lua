local metal_name = "radiant"
local metal_colour = {1.0, 1.0, 1.0}
local metal_rarity = "legendary"
local item_level = 15.0
local items = rn.mods.basegame.items

-- physical resist per item level (incl. coeff)
local physical_resist_factor = 0.02
local physical_power_factor = 1.5
local radiant_power_factor = 0.5
local max_hp_factor = 0.95
local movement_speed_factor = 0.02
local shield_resist_all_factor = 0.025

rn.renderer():add_texture("helm.radiant_crown", "basegame/res/skins/helms/radiant_crown.png")
local helm_coeff = item_level * 0.7
items[metal_name .. "_crown"] =
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
	texture = "helm.radiant_crown",
	rarity = metal_rarity
}

rn.renderer():add_texture("chest.radiant_platebody", "basegame/res/skins/chests/radiant_platebody.png")
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
	texture = "chest.radiant_platebody",
	rarity = metal_rarity
}

rn.renderer():add_texture("legs.radiant_platelegs", "basegame/res/skins/legs/radiant_platelegs.png")
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
	texture = "legs.radiant_platelegs",
	rarity = metal_rarity
}

local sword2h_coeff = item_level * 1.1
rn.renderer():add_texture("texture.solid_white", "basegame/res/textures/solid_white.png")
items[metal_name .. "_greatsword"] =
{
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.25)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, physical_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_holy_power(uuid, radiant_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, radiant_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.3)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.25)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -physical_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_holy_power(uuid, -radiant_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, -radiant_power_factor * sword2h_coeff)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.3)
	end,
	slot = rn.item.slot.right_hand,
	two_handed = false,
	colour = metal_colour,
	weapon_prefab = "weapon_model_lightbringer",
	weapon_class = "sword2h",
	emissive_map = "texture.solid_white",
	emissive_tint = rn.spell.schools.holy.colour,
	rarity = metal_rarity
}