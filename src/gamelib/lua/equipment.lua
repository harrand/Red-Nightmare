rn.equipment = {}
rn.equipment.slot =
{
	main_hand = 0,
	off_hand = 1,
	helm = 2,
	body_armour = 3,
	aura = 4
}

rn.equip = function(ent, item_name)
	local e = ent:get_element()
	local itemdata = rn.items[item_name]
	local slot = itemdata:get_equipment_slot_id()
	tz.assert(slot ~= nil)
	local object_index = nil
	if slot == rn.equipment.slot.helm then
		object_index = 6
	elseif slot == rn.equipment.slot.body_armour then
		object_index = 4
	else
		-- equipping this slot is NYI
		tz.assert(false)
	end

	local texh = rn.texture_manager():get_texture(item_name .. ".texture")
	e:object_set_texture_handle(object_index, 0, texh)
	e:object_set_visibility(object_index, true)
end