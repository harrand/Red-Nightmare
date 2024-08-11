local startscreen_lengthy = 120

rn.mods.basegame2.levels.startscreen2 =
{
	load_a_biome = function()
		local biome_id = rn.data_store():read("startscreen.biome_id") or 0
		if biome_id % 2 == 0 then
			rn.mods.basegame2.levels.startscreen2.grass_subscene()
		else
			rn.mods.basegame2.levels.startscreen2.cave_subscene()
		end
	end,
	cave_subscene = function()
		rn.renderer():set_ambient_light(0.2, 0.2, 0.225, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 1.0)

		rn.item.equip(rn.player.get(), "iron_mace")
		rn.item.equip(rn.player.get(), "basic_torch")

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
	grass_subscene = function()
		rn.renderer():set_ambient_light(0.525, 0.495, 0.495, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 0.8)
		local wallscale = 4
		local boundx = 48
		local boundy = startscreen_lengthy
		rn.item.equip(rn.player.get(), "steel_greatsword")

		local zom1 = rn.current_scene():add_entity("zombie")
		rn.entity.prefabs.sprite.set_position(zom1, -9, 5)
		rn.item.equip(zom1, "peasant_pants")
		rn.item.equip(zom1, "steel_chainmail")
		rn.item.equip(zom1, "iron_axe")
		rn.entity.prefabs.faction.set_faction(zom1, faction.player_enemy)

		local zom2 = rn.current_scene():add_entity("zombie")
		rn.item.equip(zom2, "shadow_elemental_robe")
		rn.item.equip(zom2, "shadow_elemental_hood")
		rn.item.equip(zom2, "iron_platelegs")
		rn.item.equip(zom2, "blazing_staff")
		rn.entity.prefabs.faction.set_faction(zom2, faction.player_enemy)
		rn.entity.prefabs.sprite.set_position(zom2, 9, -15)

		local ele1 = rn.current_scene():add_entity("fire_elemental")
		rn.entity.prefabs.sprite.set_position(ele1, -9, 15)
		rn.entity.prefabs.faction.set_faction(ele1, faction.player_friend)

		local ele2 = rn.current_scene():add_entity("shadow_elemental")
		rn.entity.prefabs.sprite.set_position(ele2, -6, -25)
		rn.entity.prefabs.faction.set_faction(ele2, faction.player_enemy)

		local bg = rn.current_scene():add_entity("sprite")
		local sc = math.max(boundx, boundy)
		rn.entity.prefabs.sprite.set_scale(bg, boundx, boundy)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)

		rn.entity.prefabs.dirt_path.plot_path(1, 0, (startscreen_lengthy * 0.5) - 20, "ddddddddddddddddddddd")
	end,
	on_load = function()
		if not rn.music_is_playing(0) then
			rn.play_music("basegame/res/audio/music/intro.mp3", 0)
		end

		local player = rn.current_scene():add_entity("player")
		rn.entity.prefabs.sprite.set_position(player, 0, (startscreen_lengthy * 0.5) - 20)
		rn.entity.prefabs.keyboard_controlled.set_enabled(player, false)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(player, 4)
		rn.item.equip(player, "iron_coif")
		rn.item.equip(player, "iron_chainmail")
		rn.item.equip(player, "iron_chainlegs")

		rn.level.data_write("player", player)

		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		local title_string = ren:add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
		local title_string_author = ren:add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
		local title_string_engine = ren:add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		local title_string_press_to_begin = ren:add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
		rn.level.data_write("title_string", title_string, "title_string_author", title_string_author, "title_string_press_to_begin", title_string_press_to_begin)

		rn.mods.basegame2.levels.startscreen2.load_a_biome()
	end,
	update = function(delta_seconds)
		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		local title_string, title_string_author, title_string_press_to_begin = rn.level.data_read("title_string", "title_string_author", "title_string_press_to_begin")
		ren:string_set_position(title_string, w / 2 - 250.0, h * 0.8)
		ren:string_set_position(title_string_author, w / 2 + 150, h * 0.8 - 55)
		ren:string_set_position(title_string_press_to_begin, w / 2 - 110.0, h * 0.4)

		local player = rn.level.data_read("player")
		if not rn.spell.is_casting(player) then
			rn.entity.on_move(player, 0.0, -1.0, 0.0, delta_seconds)
		end

		if rn.current_scene():contains_entity(player) then
			local px, py = rn.entity.prefabs.sprite.get_position(player)

			if py < -32.0 and not rn.level.data_read("reloading") then
				rn.data_store():set("startscreen.biome_id", (rn.data_store():read("startscreen.biome_id") or 0) + 1)
				rn.level.data_write("reloading", true)
				rn.level.reload(true)
			end
		end

		if rn.input():is_key_down("enter") then
			rn.level.load("empty")
		end
	end
}