rn.mods.basegame.prefabs.sticky =
{
	description = "Entity's global position is set to match the targetted entity every frame.",
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local stick_to_uuid = sc:entity_read(uuid, "stick_to_entity")
		if stick_to_uuid ~= nil and sc:contains_entity(stick_to_uuid) then
			local subobject = sc:entity_read(uuid, "stick_subobject")
			local offsetx = sc:entity_read(uuid, "stick_offsetx") or 0.0
			local offsety = sc:entity_read(uuid, "stick_offsety") or 0.0
			local offsetz = sc:entity_read(uuid, "stick_offsetz") or 0.0
			local posx, posy, posz = sc:entity_get_global_position(stick_to_uuid, subobject)
			sc:entity_set_global_position(uuid, posx + offsetx, posy + offsety, posz + offsetz)
		end
	end,
	stick_to = function(uuid, uuid_to_stick_to, offsetx, offsety, offsetz)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "stick_to_entity", uuid_to_stick_to)
		sc:entity_write(uuid, "stick_subobject", nil)
		sc:entity_write(uuid, "stick_offsetx", offsetx)
		sc:entity_write(uuid, "stick_offsety", offsety)
		sc:entity_write(uuid, "stick_offsetz", offsetz)
	end,
	stick_to_subobject = function(uuid, uuid_to_stick_to, subobject)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "stick_to_entity", uuid_to_stick_to)
		sc:entity_write(uuid, "stick_subobject", subobject)
	end
}
