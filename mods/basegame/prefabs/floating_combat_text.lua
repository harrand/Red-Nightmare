rn.mods.basegame.prefabs.floating_combat_text =
{
	description = "Floating Combat Text",
	set = function(uuid, posx, posy, text, colour)
		local sc = rn.current_scene()

		local text = rn.renderer():add_string(posx, posy, 12, text, colour[1], colour[2], colour[3])
		sc:entity_write(uuid, "text_uid", text)

		sc:entity_write(uuid, "text_base_x", posx)
		sc:entity_write(uuid, "text_base_y", posy)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local ren = rn.renderer()

		local text = sc:entity_read(uuid, "text_uid")
		if text ~= nil then
			local t = sc:entity_read(uuid, "timer") or 0.0
			t = t + delta_seconds
			if t > 2.5 then
				-- x seconds have passed. remove the text (and us).
				rn.renderer():remove_string(text)
				sc:remove_entity(uuid)
			else
				-- text is still alive. move text slightly upwards.
				sc:entity_write(uuid, "timer", t)
				local x = sc:entity_read(uuid, "text_base_x")
				local y = sc:entity_read(uuid, "text_base_y")

				-- convert world-space position to screen-space.
				local camx, camy = ren:get_camera_position()
				local viewx, viewy = ren:get_view_bounds()
				local w, h = tz.window():get_dimensions()
				local aspect_ratio = w / h

				local ssx = w * (((x - camx) / (viewx * aspect_ratio)) + 0.5)
				local ssy = h * (((y - camy) / viewy) + 0.5)

				rn.renderer():string_set_position(text, ssx, ssy + (t * h * 6.0 / viewy))
			end
		end
	end
}