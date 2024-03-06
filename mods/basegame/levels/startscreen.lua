rn.mods.basegame.levels.startscreen =
{
	on_load = function()
		rn.play_music("basegame/res/audio/music/intro.mp3", 0)

		local player = rn.current_scene():add_entity("player_melistra")
		rn.entity.prefabs.keyboard_controlled.set_enabled(player, false)

		rn.level.data_write("player", player)

		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		local title_string = ren:add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
		local title_string_author = ren:add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
		local title_string_engine = ren:add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		local title_string_press_to_begin = ren:add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
		rn.level.data_write("title_string", title_string, "title_string_author", title_string_author, "title_string_press_to_begin", title_string_press_to_begin)
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

		if rn.input():is_key_down("enter") then
			rn.level.load("devproc0")
		end
	end
}