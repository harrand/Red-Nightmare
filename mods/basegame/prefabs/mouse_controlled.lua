rn.mods.basegame.prefabs.mouse_controlled =
{
	description = "Entity can be controlled by mouse presses. Right now this is just movement (left-click-to-move).",
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local x, y, z = sc:entity_get_local_position(uuid)
		local inp = rn.input()

		if inp:is_mouse_down("left") then
			local mx, my = sc:get_mouse_position()
			sc:entity_write(uuid, "target_location_x", mx)
			sc:entity_write(uuid, "target_location_y", my)
		end

		local tarx = sc:entity_read(uuid, "target_location_x")
		local tary = sc:entity_read(uuid, "target_location_y")
		if tarx ~= nil and tary ~= nil then
			local dstx = tarx - x
			local dsty = tary - y
			if (dstx^2 + dsty^2) >= (sc:entity_read(uuid, ".movement_speed") or 0.0)^2 then
				rn.entity.on_move(uuid, dstx, dsty, 0.0)
			end
		end
	end
}