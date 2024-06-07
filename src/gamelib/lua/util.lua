rn = rn or {}
-- high level utility functions that are commonly used in different areas of the codebase.
rn.util =
{
	-- get direction from specified entity to the mouse cursor in world space.
	entity_direction_to_mouse = function(uuid)
		local mx, my = rn.current_scene():get_mouse_position()
		local entx, enty = rn.entity.prefabs.sprite.get_position(uuid)
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
	-- get the direction from any entity to the target
	-- if the entity is not a player, then the entity must have `target` internal variable set to a valid entity.
	-- if the entity is a player, but has no target, then the direction to the mouse cursor will be computed instead.
	entity_direction_to_target = function(uuid)
		local target = rn.util.entity_get_target(uuid)
		local is_player = rn.player.get() == uuid
		if target == nil then
			tz.assert(rn.current_scene():contains_entity(target), "target entity " .. target .. " was not found within the scene.")
			-- if is_player, get the direction to mouse
			if is_player then
				return rn.util.entity_direction_to_mouse(uuid)
			else
				-- else, error	
				tz.error()
				return 0,0
			end
		end

		-- get the position of us, and the target entity
		local entx, enty = rn.entity.prefabs.sprite.get_position(uuid)
		local tarx, tary = rn.entity.prefabs.sprite.get_position(target)
		return tarx - entx, tary - enty
	end,
}