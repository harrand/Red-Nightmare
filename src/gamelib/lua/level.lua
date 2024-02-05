rn = rn or {}
rn.level = rn.level or {}
rn.level.levels = rn.level.levels or {}

-- need:
-- rn.current_scene():clear_entities()

rn.level.load = function(name)
	local level = rn.level.levels[name]
	if level ~= nil then
		if level.on_load ~= nil then
			rn.current_scene():clear_entities()
			level.on_load()
		end
	else
		tz.report("Unknown level \"" .. name .. "\"")
		tz.assert(false);
	end
end