rn = rn or {}
rn.item = rn.item or {}
rn.item.items = rn.item.items or {}

rn.item.slot =
{
	none = 0,
	helm = 1,
	chest = 2,
	legs = 3,
	left_hand = 4,
	right_hand = 5,
	_count = 6
}

rn.item.rarity =
{
	common =
	{
		colour = {0.57735, 0.57735, 0.57735},
		pretty_name = "Common",
		impl_index = 0
	},
	uncommon = 
	{
		colour = {0.331295, 0.883452, 0.331295},
		pretty_name = "Uncommon",
		impl_index = 1
	},
	rare =
	{
		colour = {0.251498, 0.402396, 0.880242},
		pretty_name = "Rare",
		impl_index = 2
	},
	epic =
	{
		colour = {0.699127, 0.149813, 0.699127},
		pretty_name = "Epic",
		impl_index = 3
	},
	legendary =
	{
		colour = {0.991228, 0.0, 0.132164},
		pretty_name = "Legendary",
		impl_index = 4
	}
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

rn.item.get_equipped = function(uuid, slot)
	return rn.current_scene():entity_read(uuid, "equipment." .. tostring(slot))
end

rn.item.get_highest_equipped_rarity = function(uuid)
	local max_rarity = "common"
	for i=1,rn.item.slot._count-1,1 do
		local equipped = rn.item.get_equipped(uuid, i)
		if equipped ~= nil then
			local cur_rarity = rn.item.items[equipped].rarity or "common"
			if rn.item.rarity[cur_rarity].impl_index > rn.item.rarity[max_rarity].impl_index then
				max_rarity = cur_rarity
			end
		end
	end
	return max_rarity
end

rn.item.unequip_all = function(uuid)
	for i=1,rn.item.slot._count-1,1 do
		rn.item.unequip(uuid, i)
	end
end

rn.item.copy_equipment = function(uuid_from, uuid_to)
	for i=1,rn.item.slot._count-1,1 do
		local eq = rn.current_scene():entity_read(uuid_from, "equipment." .. tostring(i))
		if eq ~= nil then
			rn.item.equip(uuid_to, eq)
		end
	end
end

rn.item.move_equipment = function(uuid_from, uuid_to)
	for i=1,rn.item.slot._count-1,1 do
		local eq = rn.current_scene():entity_read(uuid_from, "equipment." .. tostring(i))
		if eq ~= nil then
			rn.item.equip(uuid_to, eq)
			rn.item.unequip(uuid_from, i)
		end
	end
end

rn.item.drop = function(item_name, x, y)
	local posx = x or 0.0
	local posy = y or 0.0
	local drop = rn.current_scene():add_entity("loot_drop")
	rn.entity.prefabs.loot_drop.set_position(drop, posx, posy)	
	rn.item.equip(drop, item_name)
end