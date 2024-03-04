rn = rn or {}

rn.static_init = function()
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Static Init")

	for prefabname, prefabdata in pairs(rn.entity.prefabs) do
		if prefabdata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(prefabname .. " - Static Init")
			prefabdata.static_init()
		end
	end

	for levelname, leveldata in pairs(rn.level.levels) do
		if leveldata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(levelname .. " - Static Init")
			leveldata.static_init()
		end
	end

	for spellname, spelldata in pairs(rn.spell.spells) do
		if spelldata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(spellname .. " - Static Init")
			spelldata.static_init()
		end
	end

	for itemname, itemdata in pairs(rn.item.items) do
		if itemdata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(itemname .. " - Static Init")
			itemdata.static_init()
		end
	end
end

rn.update = function(delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("rn.update(...)")
	-- global update. this is invoked *once* per frame, on the main thread (although you should assume the thread is unspecified)
	-- entity updates don't happen here - although they may still be in progress when this is called.
	-- global-state updates should happen here, such as current-level-advance.
	rn.level.current_level_update(delta_seconds)
end