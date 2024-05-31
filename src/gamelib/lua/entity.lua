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
	rn.current_scene():entity_write(uuid, "instantiated", true)
end

rn.entity.is_instantiated = function(uuid)
	return rn.current_scene():entity_read(uuid, "instantiated") == true
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

	for i=0,rn.item.slot._count-1,1 do
		local item = sc:entity_read(uuid, "equipment." .. i)
		if item ~= nil then
			local itemdata = rn.item.items[item]
			if itemdata.on_update ~= nil then
				itemdata.on_update(uuid, delta_seconds)
			end
		end
	end

	local stunned = rn.entity.get_stunned(uuid)
	if stunned ~= nil then
		if stunned > delta_seconds then
			rn.entity.stun(uuid, stunned - delta_seconds)
		else
			rn.entity.unstun(uuid)
		end
	end

	-- if the entity is casting a spell, that also needs to advance.
	rn.spell.advance(uuid)
	rn.buff.advance_buffs(uuid, delta_seconds)
	if not moving and sc:entity_read(uuid, "moving_last_frame") == true then
		-- entity just stopped moving.
		rn.entity.on_stop_moving(uuid)
	end
end

rn.entity.on_collision = function(uuid_a, uuid_b)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid_a) .. " and " .. tostring(uuid_b))
	obj:set_name("Lua On Collision")

	if not rn.entity.is_instantiated(uuid_a) or not rn.entity.is_instantiated(uuid_b) then return false end

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

	rn.buff.iterate_buffs(uuid_a, function(buff_name)
		local buffdata = rn.buff.buffs[buff_name]
		if buffdata.on_collision ~= nil then
			buffdata.on_collision(uuid_a, uuid_b)
		end
	end)

	-- b
	local prefab_name_b = rn.current_scene():entity_read(uuid_b, ".prefab")
	if prefab_name_b ~= nil then
		local prefab = rn.entity.prefabs[prefab_name_b]
		tz.assert(prefab ~= nil, "Missing prefab \"" .. prefab_name_b .. "\"")
		if prefab.on_collision ~= nil then
			ret = prefab.on_collision(uuid_b, uuid_a) and ret
		end
	end

	rn.buff.iterate_buffs(uuid_b, function(buff_name)
		local buffdata = rn.buff.buffs[buff_name]
		if buffdata.on_collision ~= nil then
			buffdata.on_collision(uuid_b, uuid_a)
		end
	end)
	return ret
end

rn.entity.on_move = function(uuid, xdiff, ydiff, zdiff, delta_seconds)
	tz.assert(uuid ~= nil, "on_move on nil uuid.")
	local hypot = math.sqrt(xdiff^2 + ydiff^2 + zdiff^2)
	local sc = rn.current_scene()

	local movement_speed = rn.entity.prefabs.combat_stats.get_movement_speed(uuid)
	if (xdiff == 0.0 and ydiff == 0.0 and zdiff == 0.0) or hypot == 0.0 or movement_speed == 0.0 or rn.entity.get_stunned(uuid) then
		-- ignore if we haven't actually moved
		return false
	end

	xdiff = xdiff * delta_seconds * movement_speed / hypot
	ydiff = ydiff * delta_seconds * movement_speed / hypot
	zdiff = zdiff * delta_seconds * movement_speed / hypot
	local x, y, z = sc:entity_get_local_position(uuid)
	x = x + xdiff
	y = y + ydiff
	z = z + zdiff
	sc:entity_set_local_position(uuid, x, y, z)

	-- cancel cast as we cannot cast on the move.
	if rn.spell.is_casting(uuid) then
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
	return true
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
rn.entity.on_cast_success = function(uuid, spellname, castx, casty)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_cast_success ~= nil then
				prefab.on_cast_success(uuid, spellname, castx, casty)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end

	rn.item.foreach_equipped(uuid, function(_, item_name)
		local item = rn.item.items[item_name]
		if item.on_cast ~= nil then
			item.on_cast(uuid, spellname, castx, casty)
		end
	end)
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

	rn.spell.clear(uuid)
	rn.buff.clear_all(uuid)
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

rn.entity.on_equip = function(uuid, item_name)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_equip ~= nil then
				prefab.on_equip(uuid, item_name)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

rn.entity.on_unequip = function(uuid, item_name)
	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
	if prefab_name ~= nil then
		local prefab = rn.entity.prefabs[prefab_name]
		if prefab ~= nil then
			if prefab.on_unequip ~= nil then
				prefab.on_unequip(uuid, item_name)
			end
		else
			tz.report("Missing prefab \"" .. prefab_name .. "\"")
			tz.assert(false);
		end
	end
end

rn.entity.stun = function(uuid, stun_length)
	rn.current_scene():entity_write(uuid, "stunned_duration", stun_length)
	rn.entity.prefabs.keyboard_controlled.set_enabled(uuid, false)
	rn.spell.clear(uuid)
end

rn.entity.unstun = function(uuid)
	rn.current_scene():entity_write(uuid, "stunned_duration", nil)
	rn.entity.prefabs.keyboard_controlled.set_enabled(uuid, true)
end

rn.entity.get_stunned = function(uuid)
	return rn.current_scene():entity_read(uuid, "stunned_duration")
end

rn.entity.on_struck = function(struck_uuid, striker_uuid, dmg, magic_type)
	local sc = rn.current_scene()
	if struck_uuid ~= nil and sc:contains_entity(struck_uuid) then
		local prefab_name = sc:entity_read(struck_uuid, ".prefab")
		if prefab_name ~= nil then
			local prefab = rn.entity.prefabs[prefab_name]
			if prefab.on_struck ~= nil then
				dmg = prefab.on_struck(struck_uuid, striker_uuid, dmg, magic_type)
			end
		end
	end
	return dmg
end

rn.entity.on_hit = function(hitter_uuid, victim_uuid, dmg, magic_type)
	local sc = rn.current_scene()
	if hitter_uuid ~= nil and sc:contains_entity(hitter_uuid) then
		local prefab_name = sc:entity_read(hitter_uuid, ".prefab")
		if prefab_name ~= nil then
			local prefab = rn.entity.prefabs[prefab_name]
			if prefab.on_hit ~= nil then
				dmg = prefab.on_hit(hitter_uuid, victim_uuid, dmg, magic_type)
			end
		end
	end
	return dmg
end