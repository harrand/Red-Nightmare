rn = rn or {}
rn.item = rn.item or {}
rn.item.items = rn.item.items or {}

rn.item.slot =
{
	none = 0,
	helm = 1,
	chest = 2,
	legs = 3
}

rn.item.equip = function(uuid, item_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Equip Item - \"" .. item_name .. "\"")
	local sc = rn.current_scene()

	local itemdata = rn.item.items[item_name]
	local slot = itemdata.slot or rn.item.slot.none
	--tz.report("item " .. item_name .. " is slot " .. tostring(slot))
	tz.assert(slot ~= rn.item.slot.none, "Attempt to equip item " .. item_name .. " which is not a piece of equipment.")

	-- if we're already equipping something in this slot, unequip it.
	rn.item.unequip(uuid, slot)
	-- finally equip the new item.
	sc:entity_write(uuid, "equipment." .. tostring(slot), item_name)
	rn.entity.on_equip(uuid, item_name)

	if itemdata.on_equip ~= nil then
		itemdata.on_equip(uuid)
	end

	print("entity " .. tostring(uuid) .. " equips " .. item_name .. " (slot " .. tostring(slot) .. ")")
end

rn.item.unequip = function(uuid, slot)
	local sc = rn.current_scene()
	local old_item = sc:entity_read(uuid, "equipment." .. tostring(slot))
	if old_item ~= nil then
		rn.entity.on_unequip(uuid, old_item)
		sc:entity_write(uuid, "equipment." .. tostring(slot), nil)
		print("entity " .. tostring(uuid) .. " unequips " .. old_item .. " (slot " .. tostring(slot) .. ")")

		local itemdata = rn.item.items[old_item]

		if itemdata.on_unequip ~= nil then
			itemdata.on_unequip(uuid)
		end
	end
end