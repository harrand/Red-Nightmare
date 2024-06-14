rn.mods.basegame.prefabs.text =
{
	description = "Text",
	init = function(uuid, size, r, g, b)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "text_size", size)
		sc:entity_write(uuid, "red", r)
		sc:entity_write(uuid, "green", g)
		sc:entity_write(uuid, "blue", b)
	end,
	get_string = function(uuid)
		return rn.current_scene():entity_read(uuid, "data")
	end,
	set_string = function(uuid, str)
		local sc = rn.current_scene()
		local existing_str = sc:entity_read(uuid, "data")
		if existing_str == str then return end
		local existing_text = sc:entity_read(uuid, "text_uid")
		if existing_text ~= nil then
			rn.renderer():remove_string(existing_text)
		end

		local x, y = rn.entity.prefabs.text.get_position(uuid)
		local r, g, b = rn.entity.prefabs.text.get_colour(uuid)
		local size = rn.entity.prefabs.text.get_size(uuid)
		local text = rn.renderer():add_string(x, y, size, str, r, g, b)
		sc:entity_write(uuid, "text_uid", text)
		sc:entity_write(uuid, "data", str)
	end,
	get_position = function(uuid)
		local sc = rn.current_scene()
		local x = sc:entity_read(uuid, "posx") or -999.0
		local y = sc:entity_read(uuid, "posy") or -999.0
		return x, y
	end,
	set_position = function(uuid, x, y)
		local sc = rn.current_scene()
		local text_uid = sc:entity_read(uuid, "text_uid")
		if text_uid ~= nil then
			rn.renderer():string_set_position(uuid, x, y)
		end
		local camx, camy = rn.renderer():get_camera_position()
		local viewx, viewy = rn.renderer():get_view_bounds()
		local w, h = tz.window():get_dimensions()
		local aspect_ratio = w / h

		local ssx = w * (((x - camx) / (viewx * aspect_ratio)) + 0.5)
		local ssy = h * (((y - camy) / viewy) + 0.5)
		sc:entity_write(uuid, "posx", ssx)
		sc:entity_write(uuid, "posy", ssy)
	end,
	get_size = function(uuid)
		return rn.current_scene():entity_read(uuid, "text_size")
	end,
	get_colour = function(uuid)
		local sc = rn.current_scene()
		local r = sc:entity_read(uuid, "red")
		local g = sc:entity_read(uuid, "green")
		local b = sc:entity_read(uuid, "blue")
		return r, g, b
	end,
}