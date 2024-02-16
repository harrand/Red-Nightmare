rn.mods.basegame.prefabs.sticky =
{
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local stick_to_uuid = sc:entity_read(uuid, "stick_to_entity")
		if stick_to_uuid ~= nil and sc:contains_entity(stick_to_uuid) then
			local posx, posy, posz = sc:entity_get_global_position(stick_to_uuid)
			sc:entity_set_global_position(uuid, posx, posy, posz)
		end
	end,
	stick_to = function(uuid, uuid_to_stick_to)
		rn.current_scene():entity_write(uuid, "stick_to_entity", uuid_to_stick_to)
	end
}
