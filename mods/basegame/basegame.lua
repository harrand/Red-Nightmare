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
				rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
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
				local r, g, b = rn.entity.prefabs.sprite.get_colour(me)
				local other_r, other_g, other_b = rn.entity.prefabs.sprite.get_colour(other)
				if r > other_r then
					rn.entity.prefabs.sprite.set_colour(other, r, g, b)
				end
				-- things with a 1.0 red colour component will not collide
				return r == 1.0
			end
		},
		magic_ball =
		{
			frame_count = 3,
			static_init = function()
				for i=0,rn.entity.prefabs.magic_ball.frame_count,1 do
					rn.renderer():add_texture("sprite.magicball" .. i, "basegame/res/sprites/magic_ball/magic_ball" .. i .. ".png")
				end
			end,
			pre_instantiate = function(uuid)
				return rn.entity.prefabs.sprite.pre_instantiate(uuid)
			end,
			instantiate = function(uuid)
				rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball0")
				local sc = rn.current_scene()
				sc:entity_write(uuid, "magic_type", "fire")
				sc:entity_write(uuid, ".boundary_scale", 0.5)
			end,
			update = function(uuid, delta_seconds)
				local sc = rn.current_scene()
				local magic_type = sc:entity_read(uuid, "magic_type")
				local t = sc:entity_read(uuid, "timer") or 0.0
				if t > 0 then
					tz.assert(magic_type ~= nil, "Magic ball did not have a magic type.")
				end

				-- todo: saner magic colours
				if magic_type == "fire" then
					rn.entity.prefabs.sprite.set_colour(uuid, 0.9, 0.4, 0.1)
				else
					print("PANIK")
				end
				t = t + delta_seconds
				local frame_id = math.floor((t * 10.0) % rn.entity.prefabs.magic_ball.frame_count)
				rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball" .. frame_id)
				sc:entity_write(uuid, "timer", t)


				rn.entity.prefabs.mouse_controlled.update(uuid, delta_seconds)

				local tarx = sc:entity_read(uuid, "target_location_x")
				local tary = sc:entity_read(uuid, "target_location_y")
				if tarx ~= nil and tary ~= nil then
					rn.entity.prefabs.sprite.lookat(uuid, tarx, tary, math.pi / -2.0)
				end
			end
		}
	},
	levels =
	{
		startscreen =
		{
			on_load = function()
				local morb1 = rn.current_scene():add_entity("morbius")
				--rn.renderer():set_clear_colour(1.0, 0.5, 1.0, 1.0)

				-- add a bunch more randoms
				math.randomseed(os.time())
				for i=0,128,1 do
					local morbx = rn.current_scene():add_entity("magic_ball")
					local randx = math.random(-40, 40)
					local randy = math.random(-40, 40)
					rn.entity.prefabs.sprite.set_position(morbx, randx, randy)
				end

				local w, h = tz.window():get_dimensions()
				rn.title_string = rn.renderer():add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
				rn.title_string_author = rn.renderer():add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
				rn.title_string_engine = rn.renderer():add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
				rn.title_string_press_to_begin = rn.renderer():add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO not PLAY", 1.0, 1.0, 1.0)
			end,
			update = function()
				local w, h = tz.window():get_dimensions()
				rn.title_string:set_position(w / 2 - 250.0, h * 0.8)
				rn.title_string_author:set_position(w / 2 + 150, h * 0.8 - 55)
				rn.title_string_press_to_begin:set_position(w / 2 - 110.0, h * 0.4)
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
	}
}

require("basegame/prefabs/mouse_controlled")
require("basegame/prefabs/keyboard_controlled")
require("basegame/prefabs/sprite")