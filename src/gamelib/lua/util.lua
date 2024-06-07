rn = rn or {}
-- high level utility functions that are commonly used in different areas of the codebase.
rn.util =
{
	-- get direction from specified entity to the mouse cursor in world space.
	-- if entx and enty are specified, use this as the position of the entity (useful if you want to use the position of an entity's subobject e.g the players hand). otherwise, the base transform position of the entity will be used.
	entity_direction_to_mouse = function(uuid, entx, enty)
		local mx, my = rn.current_scene():get_mouse_position()
		if entx == nil or enty == nil then
			entx, enty = rn.entity.prefabs.sprite.get_position(uuid)
		end
		return mx - entx, my - enty
	end,
	-- get direction from player entity to the mouse cursor in world space.
	player_direction_to_mouse = function()
		return rn.util.entity_direction_to_mouse(rn.player.get())
	end,
	entity_get_target = function(uuid)
		return rn.current_scene():entity_read(uuid, "target")
	end,
	entity_set_target = function(uuid, target_uuid)
		rn.current_scene():entity_write(uuid, "target", target_uuid)
	end,
	-- get the direction from any entity to the target. 
	-- if overridex and overridey are specified, that is used instead of the position of the entity. this is useful if you want to use a subobject (pass the subobject's position instead) as this will always use the entity's base transform position otherwise. see `entity_direction_to_mouse` for more info.
	-- if the entity has no target then:
		-- if the entity is the player, then the direction to the mouse cursor is retrieved instead
		-- if the entity is not the player, then the direction to the fallback entity is retrieved instead.
		-- if the fallback entity is nil, then `tarx` and `tary` represent a target location in world space.
		-- if both the fallback entity is nil, and either tarx or tary a nil, a runtime error will occur.
	entity_direction_to_target = function(uuid, fallback, tarx, tary, overridex, overridey)
		local target = rn.util.entity_get_target(uuid)
		local is_player = rn.player.get() == uuid
		if target == nil then
			-- if is_player, get the direction to mouse
			if is_player then
				return rn.util.entity_direction_to_mouse(uuid, overridex, overridey)
			else
				-- else, use the fallback.
				if fallback ~= nil then
					tz.assert(rn.current_scene():contains_entity(fallback), "entity fallback " .. fallback .. " was not found within the scene.")
					target = fallback
				end
			end
		end

		local entx, enty = rn.entity.prefabs.sprite.get_position(uuid)
		if overridex ~= nil and overridey ~= nil then
			entx = overridex
			enty = overridey
		end
		if target ~= nil then
			tz.assert(rn.current_scene():contains_entity(target), "target entity " .. target .. " was not found within the scene.")
			-- get the position of us, and the target entity
			tarx, tary = rn.entity.prefabs.sprite.get_position(target)
		else
			if tarx == nil or tary == nil then
				tz.error()
				return 0,0
			end
		end
		return tarx - entx, tary - enty
	end,
}