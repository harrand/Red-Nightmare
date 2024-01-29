rn = rn or {}
rn.entity = rn.entity or {}
rn.entity.prefabs = rn.entity.prefabs or {}

rn.entity.pre_instantiate = function(uuid, prefab_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Pre-Instantiate - \"" .. prefab_name .. "\"")

	local prefab = rn.entity.prefabs[prefab_name]
	if prefab ~= nil then
		if prefab.pre_instantiate ~= nil then
			prefab.pre_instantiate(uuid)
		end
	else
		tz.report("Missing prefab \"" .. prefab_name .. "\"")
		tz.assert(false);
	end

	tz.report("pre_instantiate " .. tostring(uuid) .. " from \"" .. prefab_name .. "\"")

	rn.current_scene():entity_set_name(uuid, prefab_name)
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

	tz.report("instantiate " .. tostring(uuid) .. " from \"" .. prefab_name .. "\"")

	rn.current_scene():entity_write(uuid, ".prefab", prefab_name);
end
