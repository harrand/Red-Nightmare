rn.mods.basegame.prefabs.faction = 
{
	get_faction = function(uuid)
		return rn.current_scene():entity_read(uuid, "faction")
	end,
	set_faction = function(uuid, faction)
		rn.current_scene():entity_write(uuid, "faction", faction)
	end,
	is_ally = function(uuid_a, uuid_b)
		local fa = rn.mods.basegame.prefabs.faction.get_faction(uuid_a)
		local fb = rn.mods.basegame.prefabs.faction.get_faction(uuid_b)
		return fa ~= nil and fa == fb
	end,
	is_enemy = function(uuid_a, uuid_b)
		return not rn.entity.prefabs.faction.is_ally(uuid_a, uuid_b)
	end,
	copy_faction = function(uuid_from, uuid_to)
		rn.entity.prefabs.faction.set_faction(uuid_to, rn.entity.prefabs.faction.get_faction(uuid_from))
	end
}

faction =
{
	player_friend = 0,
	player_enemy = 1
}