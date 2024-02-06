local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		morbius =
		{
			pre_instantiate = function(uuid)
				return "plane"
			end,
			instantiate = function(uuid)

			end,
			update = function(uuid, delta_seconds)
				local t = rn.current_scene():entity_read(uuid, "timer") or 0.0
				t = t + delta_seconds

				local x, y, z = rn.current_scene():entity_get_local_position(uuid)
				if rn.input():is_key_down("a") then
					x = x - 5.0 * delta_seconds
				end
				if rn.input():is_key_down("d") then
					x = x + 5.0 * delta_seconds
				end

				if rn.input():is_mouse_down("right") then
					x = 0; y = 0; z = 0
				end
				rn.current_scene():entity_set_local_position(uuid, x, y, z);
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

				local w, h = tz.window():get_dimensions()
				rn.title_string = rn.renderer():add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
				rn.title_string_author = rn.renderer():add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
				rn.title_string_engine = rn.renderer():add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
				rn.title_string_press_to_begin = rn.renderer():add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
			end,
			--[[
			on_update = function()
				local w, h = tz.window():get_dimensions()
				rn.title_string:set_position(w / 2 - 250.0, h * 0.8)
				rn.title_string_author:set_position(w / 2 + 150, h * 0.8 - 55)
				rn.title_string_press_to_begin:set_position(w / 2 - 110.0, h * 0.4)
			end
			--]]
		}
	}
}