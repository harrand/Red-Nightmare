local id = 6942069420
local levelstr = "startscreen"
rn.level.type[levelstr] = id

rn.level_handler[id] =
{
	on_load = function()
		local w, h = tz.window():get_dimensions()
		rn.scene():get_renderer():add_string(-400, 300, 65, "Red Nightmare", 0.6, 0.1, 0.15)
		rn.scene():get_renderer():add_string(-750, -475, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
		rn.entity_get_data(rn.player).keyboard_suppressed = true
	end
}