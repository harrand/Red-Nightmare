rn = rn or {}
rn.entity = rn.entity or {}
rn.entity.prefabs = rn.entity.prefabs or {}

rn.entity.pre_instantiate = function(uuid, prefab_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Pre-Instantiate - \"" .. prefab_name .. "\"")
	local ret = nil

	local prefab = rn.entity.prefabs[prefab_name]
	if prefab ~= nil then
		if prefab.pre_instantiate ~= nil then
			ret = prefab.pre_instantiate(uuid)
		end
	else
		tz.report("Missing prefab \"" .. prefab_name .. "\"")
		tz.assert(false);
	end

	rn.current_scene():entity_set_name(uuid, prefab_name)
	return ret
end

rn.entity.instantiate = function(uuid, prefab_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Instantiate - \"" .. prefab_name .. "\"")

	local prefab = rn.entity.prefabs[prefab_name]
	if prefab ~= nil then
		if prefab.instantiate ~= nil then
			prefab.instantiate(uuid)
		end
	else
		tz.report("Missing prefab \"" .. prefab_name .. "\"")
		tz.assert(false);
	end
end

rn.entity.update = function(uuid, delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Entity Update")

	local sc = rn.current_scene()
	local moving = sc:entity_read(uuid, "moving")
	sc:entity_write(uuid, "moving_last_frame", moving)
	sc:entity_write(uuid, "moving", false)

	local prefab_name = sc:entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.update ~= nil then
				prefab.update(uuid, delta_seconds)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end

	-- if the entity is casting a spell, that also needs to advance.
	rn.spell.advance(uuid)
	if not moving and sc:entity_read(uuid, "moving_last_frame") == true then
		-- entity just stopped moving.
		rn.entity.on_stop_moving(uuid)
	end
end

rn.entity.on_collision = function(uuid_a, uuid_b)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid_a) .. " and " .. tostring(uuid_b))
	obj:set_name("Lua On Collision")

	local ret = true
	-- a
	local prefab_name_a = rn.current_scene():entity_read(uuid_a, ".prefab")
	if prefab_name_a ~= nil then
		local prefab = rn.entity.prefabs[prefab_name_a]
		tz.assert(prefab ~= nil, "Missing prefab \"" .. prefab_name_a .. "\"")
		if prefab.on_collision ~= nil then
			ret = prefab.on_collision(uuid_a, uuid_b) and ret
		end
	end

	-- b
	local prefab_name_b = rn.current_scene():entity_read(uuid_b, ".prefab")
	if prefab_name_b ~= nil then
		local prefab = rn.entity.prefabs[prefab_name_b]
		tz.assert(prefab ~= nil, "Missing prefab \"" .. prefab_name_b .. "\"")
		if prefab.on_collision ~= nil then
			ret = prefab.on_collision(uuid_b, uuid_a) and ret
		end
	end
	return ret
end

rn.entity.on_move = function(uuid, xdiff, ydiff, zdiff)
	local hypot = math.sqrt(xdiff^2 + ydiff^2 + zdiff^2)
	local sc = rn.current_scene()

	local movement_speed = sc:entity_read(uuid, ".movement_speed") or 0.0
	if (xdiff == 0.0 and ydiff == 0.0 and zdiff == 0.0) or hypot == 0.0 or movement_speed == 0.0 then
		-- ignore if we haven't actually moved
		return
	end

	xdiff = xdiff * movement_speed / hypot
	ydiff = ydiff * movement_speed / hypot
	zdiff = zdiff * movement_speed / hypot
	local x, y, z = sc:entity_get_local_position(uuid)
	x = x + xdiff
	y = y + ydiff
	z = z + zdiff
	sc:entity_set_local_position(uuid, x, y, z)

	-- cancel cast as we cannot cast on the move.
	if rn.spell.is_casting(uuid) then
		print("cast cancelled due to movement.")
		rn.spell.clear(uuid)
	end

	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_move ~= nil then
				prefab.on_move(uuid, xdiff, ydiff, zdiff)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
	sc:entity_write(uuid, "moving", true)
end

rn.entity.on_stop_moving = function(uuid)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_stop_moving ~= nil then
				prefab.on_stop_moving(uuid)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end

end

-- invoked when an entity begins casting a spell.
rn.entity.on_cast_begin = function(uuid, spellname)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_cast_begin ~= nil then
				prefab.on_cast_begin(uuid, spellname)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

-- invoked when an entity is no longer casting a spell (happens if the cast is cancelled or completes)
rn.entity.on_cast_end = function(uuid)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_cast_end ~= nil then
				prefab.on_cast_end(uuid)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

-- invoked when an entity is finishes a spell (successful completion, not if the cast is cancelled)
rn.entity.on_cast_success = function(uuid)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_cast_success ~= nil then
				prefab.on_cast_success(uuid)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

rn.entity.on_death = function(uuid, dmg, magic_type, enemy_uuid)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_death ~= nil then
				prefab.on_death(uuid, dmg, magic_type, enemy_uuid)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

rn.entity.on_remove = function(uuid)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_remove ~= nil then
				prefab.on_remove(uuid, dmg, magic_type, enemy_uuid)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end