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

	local prefab_name = rn.current_scene():entity_read(uuid, ".prefab")
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