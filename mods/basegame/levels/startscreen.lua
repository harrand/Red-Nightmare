local startscreen_lengthy = 120

rn.mods.basegame.levels.startscreen =
{
	cave_subscene = function()
		rn.renderer():set_ambient_light(0.3, 0.3, 0.4, 1.0)

		local wallscale = 4
		local boundx = 48
		local boundy = startscreen_lengthy
		boundx = math.floor((boundx * 0.5) - wallscale)
		boundy = math.floor((boundy * 0.5) - wallscale)
		for x=-boundx,boundx,wallscale do
			for y=-boundy,boundy,wallscale do
				if x >= -1.0 and x <= 1.0 then
					-- do nothing. it'll be in the players way
				elseif x == -boundx or x >= (boundx - wallscale + 1) or y == -boundy or y >= (boundy - wallscale + 1) then
					local ent = rn.current_scene():add_entity("wall")
					rn.entity.prefabs.sprite.set_position(ent, x, y)
					rn.entity.prefabs.sprite.set_scale(ent, wallscale * 0.5)
					rn.entity.prefabs.sprite.set_texture(ent, "material.darkstone")
					rn.entity.prefabs.sprite.set_normal_map(ent, "material.darkstone_normals")
				else
					-- randomly 10%...
					local randval = math.random()
					if randval <= 0.1 then
						-- 2.5% chance of spawning a loot chest
						if randval <= 0.025 then
							local rarity = "common"
							if randval <= 0.001 then
								rarity = "legendary"
							elseif randval <= 0.005 then
								rarity = "epic"
							elseif randval <= 0.01 then
								rarity = "rare"
							end
							local ent = rn.current_scene():add_entity("loot_chest")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
							rn.entity.prefabs.loot_chest.add_random_loot(ent, rarity)
						else
							-- 7.5% chance of just a wall
							local ent = rn.current_scene():add_entity("wall")
							rn.entity.prefabs.sprite.set_texture(ent, "material.darkstone")
							rn.entity.prefabs.sprite.set_normal_map(ent, "material.darkstone_normals")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
							rn.entity.prefabs.sprite.set_scale(ent, wallscale * 0.5)
						end
					end
				end
			end
		end

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, math.max(boundx, boundy))
		rn.entity.prefabs.sprite.set_texture(bg, "background.blackrock")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.blackrock_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
	end,
	on_load = function()
		if not rn.music_is_playing(0) then
			rn.play_music("basegame/res/audio/music/intro.mp3", 0)
		end

		local player = rn.current_scene():add_entity("player_melistra")
		rn.entity.prefabs.sprite.set_position(player, 0, (startscreen_lengthy * 0.5) - 20)
		rn.entity.prefabs.keyboard_controlled.set_enabled(player, false)
		rn.entity.prefabs.combat_stats.set_pct_more_movement_speed(player, -0.2)
		rn.item.equip(player, "iron_coif")
		rn.item.equip(player, "iron_chainmail")
		rn.item.equip(player, "iron_chainlegs")
		rn.item.equip(player, "iron_sword")
		rn.item.equip(player, "basic_torch")

		rn.level.data_write("player", player)

		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		local title_string = ren:add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
		local title_string_author = ren:add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
		local title_string_engine = ren:add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		local title_string_press_to_begin = ren:add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
		rn.level.data_write("title_string", title_string, "title_string_author", title_string_author, "title_string_press_to_begin", title_string_press_to_begin)

		rn.mods.basegame.levels.startscreen.cave_subscene()
	end,
	update = function(delta_seconds)
		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		local title_string, title_string_author, title_string_press_to_begin = rn.level.data_read("title_string", "title_string_author", "title_string_press_to_begin")
		ren:string_set_position(title_string, w / 2 - 250.0, h * 0.8)
		ren:string_set_position(title_string_author, w / 2 + 150, h * 0.8 - 55)
		ren:string_set_position(title_string_press_to_begin, w / 2 - 110.0, h * 0.4)

		local player = rn.level.data_read("player")
		rn.entity.on_move(player, 0.0, -1.0, 0.0, delta_seconds)

		if rn.current_scene():contains_entity(player) then
			local px, py = rn.entity.prefabs.sprite.get_position(player)
			if py < -52.0 then
				rn.level.reload(true)
			end
		end

		if rn.input():is_key_down("enter") then
			rn.level.load("clans_camp")
		end
	end
}