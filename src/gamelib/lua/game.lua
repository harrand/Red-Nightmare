rn = rn or {}

rn.static_init = function()
	for prefabname, prefabdata in pairs(rn.entity.prefabs) do
		if prefabdata.static_init ~= nil then
			prefabdata.static_init()
		end
	end
end

rn.update = function(delta_seconds)
	-- global update. this is invoked *once* per frame, on the main thread (although you should assume the thread is unspecified)
	-- entity updates don't happen here - although they may still be in progress when this is called.
	-- global-state updates should happen here, such as current-level-advance.
end