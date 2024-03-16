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
	
	rn_impl_static_armour_load()
end

rn.update = function(delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("rn.update(...)")
	-- global update. this is invoked *once* per frame, on the main thread (although you should assume the thread is unspecified)
	-- entity updates don't happen here - although they may still be in progress when this is called.
	-- global-state updates should happen here, such as current-level-advance.
	rn.level.current_level_update(delta_seconds)
	rn.camera_follow_player(delta_seconds)
end

rn.camera_follow_player = function(delta_seconds)
	local player_uuid = rn.level.data_read("player")
	if player_uuid == nil or not rn.current_scene():contains_entity(player_uuid) then return end

	local camx, camy = rn.renderer():get_camera_position()
	local px, py = rn.entity.prefabs.sprite.get_position(player_uuid)
	local dstx = px - camx
	local dsty = py - camy
	local avgdst = (math.abs(dstx) + math.abs(dsty)) / 2.0

	local viewx, viewy = rn.renderer():get_view_bounds()
	local avgview = (viewx + viewy) / 2.0
	-- if the tracked player is at least half the screen away from the middle.
	local is_nearly_out = (avgdst >= (avgview * 0.25 * 0.5))
	local is_absolutely_miles_out = (avgdst >= (avgview * 0.5))
	if is_absolutely_miles_out == true then
		-- we're super far out. just teleport camera to the player.
		rn.renderer():set_camera_position(px, py)
	elseif is_nearly_out == true then
		-- we kinda need to catch up
		-- lerp based on delta_seconds
		camx = camx + (dstx * delta_seconds)
		camy = camy + (dsty * delta_seconds)
		rn.renderer():set_camera_position(camx, camy)
	end
end