rn.mods.basegame.prefabs.keyboard_controlled =
{
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "control.forward", "w")
		sc:entity_write(uuid, "control.left", "a")
		sc:entity_write(uuid, "control.right", "d")
		sc:entity_write(uuid, "control.backward", "s")
		sc:entity_write(uuid, "control.enabled", true)
		for i=1,9,1 do
			sc:entity_write(uuid, "control." .. tostring(i), tostring(i))
		end
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local x, y, z = sc:entity_get_local_position(uuid)
		local inp = rn.input()

		local control_forward = sc:entity_read(uuid, "control.forward")
		local control_left = sc:entity_read(uuid, "control.left")
		local control_right = sc:entity_read(uuid, "control.right")
		local control_backward = sc:entity_read(uuid, "control.backward")
		local control_enabled = sc:entity_read(uuid, "control.enabled")

		if control_enabled then
			for i=1,9,1 do
				local istr = tostring(math.floor(i))
				local control_action = sc:entity_read(uuid, "control." .. istr)
				-- we expect control_action to be a string representation of an *integer*
				-- however if its 1, then the string could easily be `1.0`
				-- so we string -> number -> string. i know...
				if inp:is_key_down(tostring(math.floor(tonumber(control_action)))) then
					local cast_name = sc:entity_read(uuid, "action." .. istr)
					if cast_name ~= nil then
						rn.spell.cast(uuid, cast_name)
					end
				end
			end
		end

		local movement_speed = 5.0
		local moved = false

		if control_enabled and inp:is_key_down(control_left) then
			x = x - movement_speed * delta_seconds
			moved = true
		end
		if control_enabled and inp:is_key_down(control_right) then
			x = x + movement_speed * delta_seconds
			moved = true
		end
		if control_enabled and inp:is_key_down(control_forward) then
			y = y + movement_speed * delta_seconds
			moved = true
		end
		if control_enabled and inp:is_key_down(control_backward) then
			y = y - movement_speed * delta_seconds
			moved = true
		end
		
		if moved then
			sc:entity_set_local_position(uuid, x, y, z);
		end
	end,
	bind_spell = function(uuid, action_id, spell_name)
		rn.current_scene():entity_write(uuid, "action." .. tostring(math.floor(action_id)), spell_name)
	end
}