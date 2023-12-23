rn.equipment = {}
rn.equipment.slot =
{
	main_hand = 1,
	off_hand = 2,
	helm = 3,
	body_armour = 4,
	legs = 5,
	aura = 6,
	_count = 7
}

rn._internal_equipment_slot_object_mapping =
{
	nil,
	nil,
	7,
	5,
	9,
	nil
}

rn.entity_equipment = {}

rn.get_equipped_item = function(ent, slot)
	rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
	return rn.entity_equipment[ent:uid()][slot]
end

-- copy over all equipment of ent to ent2, so they both wear the same stuff.
rn.equipment_copy = function(ent, ent2)
	for i=1,rn.equipment.slot._count-1,1 do
		rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
		local item = rn.entity_equipment[ent:uid()][i]
		if item ~= nil and rn._internal_equipment_slot_object_mapping[i] ~= nil then
			rn.equip(ent2, item)
		end
	end
end

rn.unequip = function(ent, slot_id)
	local e = ent:get_element()
	rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
	rn.entity_equipment[ent:uid()][slot_id] = nil

	local object_index = rn._internal_equipment_slot_object_mapping[slot_id]
	tz.assert(object_index ~= nil)
	e:object_set_visibility(object_index, false)
end

rn.unequip_and_drop = function(ent, slot_id)
	rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
	local item = rn.entity_equipment[ent:uid()][slot_id]
	if item == nil then
		return
	end
	local x, y = ent:get_element():get_position()
	rn.unequip(ent, slot_id)
	rn.drop_item_at(item, x, y)	
end

rn.unequip_and_drop_all = function(ent)
	if rn.entity_get_data(ent).impl.drop_items_on_death == false then return end
	local x, y = ent:get_element():get_position()
	for i=1,rn.equipment.slot._count-1,1 do
		rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
		local item = rn.entity_equipment[ent:uid()][i]
		if item ~= nil and rn._internal_equipment_slot_object_mapping[i] ~= nil then
			rn.unequip(ent, i)
			rn.drop_item_at(item, x, y)	
		end
	end
end

rn.equip = function(ent, item_name)
	local e = ent:get_element()
	local itemdata = rn.items[item_name]
	local slot = itemdata:get_equipment_slot_id()
	tz.assert(slot ~= nil)
	rn.entity_equipment[ent:uid()] = rn.entity_equipment[ent:uid()] or {}
	rn.entity_equipment[ent:uid()][slot] = item_name
	local object_index = rn._internal_equipment_slot_object_mapping[slot]
	tz.assert(object_index ~= nil)

	local texh = rn.texture_manager():get_texture(item_name .. ".texture")
	e:object_set_texture_handle(object_index, 0, texh)
	local r, g, b = itemdata:get_texture_tint()
	e:object_set_colour_tint(object_index, r, g, b)
	e:object_set_visibility(object_index, true)
end