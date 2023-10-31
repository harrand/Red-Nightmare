local id = 3
local typestr = "loot drop"
-- intended to be used to display dropped items on the floor.
-- for each dropped item, you can equip it onto this mannequin and it will display as if being worn by an invisible humanoid.
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	preinit = function(ent)
		ent:set_name("Dropped Item")
		ent:set_model(rn.model.humanoid)
		rn.entity.data[ent:uid()] =
		{
			collided_this_update = false
		}
	end,
	postinit = function(ent)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.3)
		ent:get_element():object_set_visibility(3, false)
		ent:get_element():face_forward()
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		rn.for_each_collision(ent, function(ent2)
			if not data.collided_this_update and ent2:get_model() == rn.model.humanoid and ent2:get_type() ~= ent:get_type() and not ent2:is_dead() then
				-- equip whatever we're wearing onto ent2
				-- iterate through each slot
				for i=1,rn.equipment.slot._count-1,1 do
					-- not all slots are implemented, so skip thsoe	
					if rn._internal_equipment_slot_object_mapping[i] ~= nil then
						local equip = rn.get_equipped_item(ent, i)
						if equip ~= nil then
							-- make ent2 equip this
							rn.equip(ent2, equip)
							print(ent2:get_name() .. " has picked up " .. equip)
						end
					end
				end
				-- then we die :)
				data.collided_this_update = true
			end
		end)
		if data.collided_this_update then
			rn.scene():remove_uid(ent:uid())
		end
	end
}