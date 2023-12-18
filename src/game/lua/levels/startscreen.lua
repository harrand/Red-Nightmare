local id = 6942069420
local levelstr = "startscreen"
rn.level.type[levelstr] = id

rn.level_handler[id] =
{
	on_load = function()
		local w, h = tz.window():get_dimensions()
		rn.title_string = rn.scene():get_renderer():add_string(0.0, 0.0, 40, "Red Nightmare", 0.6, 0.1, 0.15)
		rn.title_string_author = rn.scene():get_renderer():add_string(0.0, 0.0, 12, "by Harrand", 0.9, 0.3, 0.35)
		rn.title_string_engine = rn.scene():get_renderer():add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		rn.title_string_press_to_begin = rn.scene():get_renderer():add_string(0.0, 0.0, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
		local pdata = rn.entity_get_data(rn.player)
		pdata.keyboard_suppressed = true
		pdata.mouse_suppressed = true
		pdata.direction_force = "forward"
	end,
	on_update = function()
		local w, h = tz.window():get_dimensions()
		rn.title_string:set_position(w / 2 - 250.0, h * 0.8)
		rn.title_string_author:set_position(w / 2 + 150, h * 0.8 - 55)
		rn.title_string_press_to_begin:set_position(w / 2 - 110.0, h * 0.4)

		local x, y = rn.player:get_element():get_position()
		local x2, y2 = rn.level_bg:get_element():get_position()
		if y2 >= y + 32 then
			rn.level_bg:get_element():set_position(x2, y)
		end

		if rn.is_key_down("enter") then
			rn.load_level{name = "blanchfield"}
		end
	end
}