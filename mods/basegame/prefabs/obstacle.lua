rn.mods.basegame.prefabs.obstacle =
{
	instantiate = function(uuid)
		rn.current_scene():entity_write(uuid, "obstacle", true)
		rn.current_scene():entity_write(uuid, "mass", 9999999999)
	end,
	on_collision = function(me, other)
		-- collision response if other is not an obstacle.
		return not rn.entity.prefabs.obstacle.is_obstacle(other)
	end,
	is_obstacle = function(uuid)
		return rn.current_scene():entity_read(uuid, "obstacle") == true
	end
}