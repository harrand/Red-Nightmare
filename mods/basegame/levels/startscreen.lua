rn.mods.basegame.levels.startscreen =
{
	on_load = function()
		rn.play_music("basegame/res/audio/music/intro.mp3", 0)

		rn.player = rn.current_scene():add_entity("player_melistra")
		rn.entity.prefabs.keyboard_controlled.set_enabled(rn.player, false)

		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		rn.title_string = ren:add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
		rn.title_string_author = ren:add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
		rn.title_string_engine = ren:add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		rn.title_string_press_to_begin = ren:add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO not PLAY", 1.0, 1.0, 1.0)
	end,
	update = function(delta_seconds)
		local w, h = tz.window():get_dimensions()
		local ren = rn.renderer()
		ren:string_set_position(rn.title_string, w / 2 - 250.0, h * 0.8)
		ren:string_set_position(rn.title_string_author, w / 2 + 150, h * 0.8 - 55)
		ren:string_set_position(rn.title_string_press_to_begin, w / 2 - 110.0, h * 0.4)

		rn.entity.on_move(rn.player, 0.0, -1.0, 0.0, delta_seconds)

		if rn.input():is_key_down("enter") then
			rn.level.load("dev0")
		end
	end
}