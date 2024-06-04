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
			if sc:entity_read(uuid, "copy_rotation") then
				sc:entity_set_global_rotation(uuid, sc:entity_get_global_rotation(stick_to_uuid, subobject))
			end
			if sc:entity_read(uuid, "copy_scale") then
				sc:entity_set_global_scale(uuid, sc:entity_get_global_scale(stick_to_uuid, subobject))
			end
		end
	end,
	stick_to = function(uuid, uuid_to_stick_to, offsetx, offsety, offsetz, full_transform)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "stick_to_entity", uuid_to_stick_to)
		sc:entity_write(uuid, "stick_subobject", nil)
		sc:entity_write(uuid, "stick_offsetx", offsetx)
		sc:entity_write(uuid, "stick_offsety", offsety)
		sc:entity_write(uuid, "stick_offsetz", offsetz)
		sc:entity_write(uuid, "full_transform", full_transform)
	end,
	get_stuck_to = function(uuid)
		return rn.current_scene():entity_read(uuid, "stick_to_entity")
	end,
	stick_to_subobject = function(uuid, uuid_to_stick_to, subobject, copy_rotation, copy_scale)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "stick_to_entity", uuid_to_stick_to)
		sc:entity_write(uuid, "stick_subobject", subobject)
		sc:entity_write(uuid, "copy_rotation", copy_rotation)
		sc:entity_write(uuid, "copy_scale", copy_scale)
	end,
	set_offset = function(uuid, offsetx, offsety, offsetz)
		local sc = rn.current_scene()
		if offsetx ~= nil then
			sc:entity_write(uuid, "stick_offsetx", offsetx)	
		end
		if offsety ~= nil then
			sc:entity_write(uuid, "stick_offsety", offsety)	
		end
		if offsetz ~= nil then
			sc:entity_write(uuid, "stick_offsetz", offsetz)	
		end
	end,
}
