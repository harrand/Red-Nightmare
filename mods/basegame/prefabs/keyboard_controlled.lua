rn.mods.basegame.prefabs.keyboard_controlled =
{
	description = "Entity can be controlled by keybinds. Namely movement (default wasd) and upto nine spells (1-9) bound.",
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "control.forward", "w")
		sc:entity_write(uuid, "control.left", "a")
		sc:entity_write(uuid, "control.right", "d")
		sc:entity_write(uuid, "control.backward", "s")
		for i=1,9,1 do
			sc:entity_write(uuid, "control." .. tostring(i), tostring(i))
		end
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local inp = rn.input()

		local control_forward = sc:entity_read(uuid, "control.forward")
		local control_left = sc:entity_read(uuid, "control.left")
		local control_right = sc:entity_read(uuid, "control.right")
		local control_backward = sc:entity_read(uuid, "control.backward")
		local control_enabled = rn.entity.prefabs.keyboard_controlled.get_enabled(uuid)

		if control_enabled then
			for i=1,9,1 do
				local istr = tostring(math.floor(i))
				local control_action = sc:entity_read(uuid, "control." .. istr)
				-- we expect control_action to be a string representation of an *integer*
				-- however if its 1, then the string could easily be `1.0`
				-- so we string -> number -> string. i know...
				if control_action ~= nil and inp:is_key_down(tostring(math.floor(tonumber(control_action)))) then
					local cast_name = sc:entity_read(uuid, "action." .. istr)
					if cast_name ~= nil then
						rn.spell.cast(uuid, cast_name)
					end
				end
			end

			for slot, slotdata in pairs(rn.spell.slot) do
				local keybind = slotdata.default_keybind
				local spellname = rn.entity.prefabs.spell_slots.get_spell(uuid, slot)
				if inp:is_key_down(keybind) then
					rn.entity.prefabs.spell_slots.cast_spell_at_slot(uuid, slot)
				end
			end
		end

		local moved = false

		local xdiff = 0
		local ydiff = 0
		if control_enabled and control_left ~= nil and inp:is_key_down(control_left) then
			xdiff = -1
			moved = true
		end
		if control_enabled and control_right ~= nil and inp:is_key_down(control_right) then
			xdiff = 1
			moved = true
		end
		if control_enabled and control_forward ~= nil and inp:is_key_down(control_forward) then
			ydiff = 1
			moved = true
		end
		if control_enabled and control_backward ~= nil and inp:is_key_down(control_backward) then
			ydiff = -1
			moved = true
		end
		
		if moved then
			rn.entity.on_move(uuid, xdiff, ydiff, 0.0, delta_seconds)
		end
	end,
	bind_spell = function(uuid, action_id, spell_name)
		rn.current_scene():entity_write(uuid, "action." .. tostring(math.floor(action_id)), spell_name)
	end,
	get_enabled = function(uuid)
		local val = rn.current_scene():entity_read(uuid, "control.enabled")
		if val == nil then return true end
		return val
	end,
	set_enabled = function(uuid, boolval)
		rn.current_scene():entity_write(uuid, "control.enabled", boolval)
	end
}