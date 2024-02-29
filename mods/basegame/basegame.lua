rn.mods.basegame =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		morbius =
		{
			static_init = function()
				print("morb to begin")
				rn.renderer():add_texture("effect.consecrate", "basegame/res/sprites/consecrate.png")
			end,
			pre_instantiate = function(uuid)
				return rn.entity.prefabs.sprite.pre_instantiate(uuid)
			end,
			instantiate = function(uuid)
				rn.entity.prefabs.sprite.instantiate(uuid)
				rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
				rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 1, "lesser_firebolt")
				rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 2, "morb")
				rn.current_scene():entity_write(uuid, "morbing", false)
				rn.current_scene():entity_write(uuid, "age", 25)
				rn.current_scene():entity_write(uuid, "timer", 0.0)
				rn.current_scene():entity_write(uuid, "personality", nil)
				rn.current_scene():entity_write(uuid, "mass", 1.0)
				rn.entity.prefabs.sprite.set_colour(uuid, 0.0, 0.3, 0.6)
				rn.entity.prefabs.sprite.set_texture(uuid, "effect.consecrate")
			end,
			update = function(uuid, delta_seconds)
				rn.entity.prefabs.keyboard_controlled.update(uuid, delta_seconds)
				local morbing = rn.current_scene():entity_read(uuid, "morbing")
				local t = rn.current_scene():entity_read(uuid, "timer") or 0
				t = t + delta_seconds
				rn.current_scene():entity_write(uuid, "timer", t)
				if morbing == true then
					print("MORBING TIME")
				end

				if rn.input():is_key_down("l") then
					rn.entity.prefabs.sprite.set_rotation(uuid, rn.entity.prefabs.sprite.get_rotation(uuid) - delta_seconds * 2.5)
				end


				if rn.input():is_key_down("k") then
					rn.entity.prefabs.sprite.set_scale(uuid, rn.entity.prefabs.sprite.get_scale(uuid) * (1.0 - delta_seconds))
				end

				if rn.input():is_key_down("i") then
					rn.entity.prefabs.sprite.set_scale(uuid, rn.entity.prefabs.sprite.get_scale(uuid) * (1.0 + delta_seconds))
				end

				if rn.input():is_key_down("j") then
					rn.entity.prefabs.sprite.set_rotation(uuid, rn.entity.prefabs.sprite.get_rotation(uuid) + delta_seconds * 2.5)
				end

				-- look at the mouse
				local mx, my = rn.current_scene():get_mouse_position()
				rn.entity.prefabs.sprite.lookat(uuid, mx, my, 0.0)
			end,
			on_collision = function(me, other)
				return true
			end
		},
	},
	levels =
	{
		startscreen =
		{
			static_init = function()
				rn.renderer():add_texture("background.grassy", "basegame/res/textures/background_grassy.png")
				rn.renderer():add_texture("background.grassy_normals", "basegame/res/textures/background_grassy_normals.png")
			end,
			on_load = function()
				local bg = rn.current_scene():add_entity("sprite")
				rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
				rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
				rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
				rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
				local morb1 = rn.current_scene():add_entity("player_melistra")
				rn.item.equip(morb1, "iron_sallet")
				--rn.renderer():set_clear_colour(1.0, 0.5, 1.0, 1.0)

				-- add a bunch more randoms
				math.randomseed(os.time())
				for i=0,128,1 do
					local ty = "firebolt"
					if i%2 == 0 then ty = "frostbolt" end
					local morbx = rn.current_scene():add_entity(ty)
					rn.entity.prefabs.magic_ball_base.set_damage(morbx, 2)
					local randx = math.random(-40, 40)
					local randy = math.random(-40, 40)
					rn.entity.prefabs.sprite.set_position(morbx, randx, randy)
				end

				local w, h = tz.window():get_dimensions()
				local ren = rn.renderer()
				rn.title_string = ren:add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
				rn.title_string_author = ren:add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
				rn.title_string_engine = ren:add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
				rn.title_string_press_to_begin = ren:add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO not PLAY", 1.0, 1.0, 1.0)

				rn.play_music("basegame/res/audio/music/intro.mp3", 0)
			end,
			update = function()
				local w, h = tz.window():get_dimensions()
				local ren = rn.renderer()
				ren:string_set_position(rn.title_string, w / 2 - 250.0, h * 0.8)
				ren:string_set_position(rn.title_string_author, w / 2 + 150, h * 0.8 - 55)
				ren:string_set_position(rn.title_string_press_to_begin, w / 2 - 110.0, h * 0.4)
			end
		}
	},
	spells = 
	{
		morb = 
		{
			cast_duration = 2.0,
			advance = function(uuid)
				--print("MORBING...")
			end,
			finish = function(uuid)
				rn.current_scene():entity_write(uuid, "morbing", true)
			end
		}
	},
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
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.25)
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
			end,
			on_unequip = function(uuid)
				rn.entity.prefabs.combat_stats.apply_flat_increased_physical_resist(uuid, -0.15)
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
require("basegame/prefabs/cast_buildup")
require("basegame/prefabs/combat_stats")
require("basegame/prefabs/timed_despawn")
require("basegame/prefabs/obstacle")
require("basegame/prefabs/invisible_wall")
require("basegame/prefabs/floating_combat_text")
require("basegame/prefabs/health_bar")

require("basegame/prefabs/players/player_melistra")

require("basegame/spells/flash_of_light")
require("basegame/spells/lesser_firebolt")
require("basegame/spells/lesser_frostbolt")


require("basegame/items/fiery_hauberk")
require("basegame/items/white_legion_helm")