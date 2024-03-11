rn = rn or {}
rn.player = rn.player or {}

rn.player.get = function()
	return rn.level.data_read("player") or nil
end

rn.player.stash = function()
	local player = rn.player.get()
	if player == nil then return end

	local equipment = {}
	rn.item.foreach_equipped(player, function(i, item_name)
		table.insert(equipment, "player.equipment." .. tostring(i))
		table.insert(equipment, item_name)
	end)

	rn.data_store():edit_some(table.unpack(equipment))
end

rn.player.unstash = function()
	local player = rn.player.get()
	if player == nil then return end

	for i=1,rn.item.slot._count-1,1 do
		local equipped = rn.data_store():read("player.equipment." .. tostring(i))
		if equipped ~= nil then
			rn.item.equip(player, equipped)
		end
	end
	rn.data_store():remove_all_of("player.")
end