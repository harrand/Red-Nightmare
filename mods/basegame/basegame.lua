rn.mods.basegame =
{
	description = "Base game content for Red Nightmare",
	prefabs = {},
	levels = {},
	spells = 
	{},
	items =
	{
		iron_sallet =
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
		},
		iron_chainmail =
		{
			static_init = function()
				rn.renderer():add_texture("chest.chainmail", "basegame/res/skins/chests/chainmail.png")
			end,
			on_equip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, 0.25)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 5)
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.25)
				rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -5)
			end,
			slot = rn.item.slot.chest,
			colour = {0.5, 0.5, 0.5},
			texture = "chest.chainmail",
		},
		iron_chainlegs =
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
		},
	}
}

require("basegame/prefabs/mouse_controlled")
require("basegame/prefabs/keyboard_controlled")
require("basegame/prefabs/light_emitter")
require("basegame/prefabs/sticky")
require("basegame/prefabs/sprite")
require("basegame/prefabs/bipedal")
require("basegame/prefabs/magicbolt")
require("basegame/prefabs/allure_of_flame")
require("basegame/prefabs/cast_buildup")
require("basegame/prefabs/combat_stats")
require("basegame/prefabs/loot_drop")
require("basegame/prefabs/timed_despawn")
require("basegame/prefabs/obstacle")
require("basegame/prefabs/wall")
require("basegame/prefabs/invisible_wall")
require("basegame/prefabs/loot_chest")
require("basegame/prefabs/floating_combat_text")
require("basegame/prefabs/health_bar")
require("basegame/prefabs/melee_swing_area")
require("basegame/prefabs/enemy_ai")
require("basegame/prefabs/zombie")
require("basegame/prefabs/elemental")
require("basegame/prefabs/portal")

require("basegame/prefabs/players/player_melistra")

require("basegame/prefabs/weapon_models/morning_star")
require("basegame/prefabs/weapon_models/small_shield")
require("basegame/prefabs/weapon_models/sword")
require("basegame/prefabs/weapon_models/torch")

require("basegame/spells/melee")
require("basegame/spells/allure_of_flames")
require("basegame/spells/firestorm")
require("basegame/spells/flash_of_light")
require("basegame/spells/lesser_magicbolt")
require("basegame/spells/summon_zombie")

require("basegame/items/basic_torch")
require("basegame/items/fiery_hauberk")
require("basegame/items/holy_water_sprinkler")
require("basegame/items/insin")
require("basegame/items/small_iron_shield")
require("basegame/items/steel_longsword")
require("basegame/items/white_legion_helm")
require("basegame/items/white_legion_platebody")
require("basegame/items/white_legion_platelegs")
require("basegame/items/elemental_circlet")

require("basegame/levels/startscreen")
require("basegame/levels/dev0")
require("basegame/levels/devproc0")