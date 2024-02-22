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
			local movement_speed = 5.0
			local dstx = tarx - x
			local dsty = tary - y
			local hypot = math.sqrt(dstx^2 + dsty^2)
			-- if hypot is zero then it's exactly at the position - no need to move.
			if hypot > 0.0 and hypot >= (delta_seconds * movement_speed) then
				local xdiff = ((dstx / hypot) * delta_seconds * movement_speed)
				local ydiff = ((dsty / hypot) * delta_seconds * movement_speed)
				rn.entity.on_move(uuid, xdiff, ydiff, 0.0)
			end
		end
	end
}