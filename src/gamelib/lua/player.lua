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
	local hp_lost = rn.current_scene():entity_read(player, "hp_lost")

	rn.data_store():edit_some(table.unpack(equipment))
	rn.data_store():set("player.hp_lost", hp_lost or 0)
	for slot, slotdata in pairs(rn.spell.slot) do
		local spellname = rn.entity.prefabs.spell_slots.get_spell(player, slot)
		rn.data_store():set("player_slot." .. tostring(slot), spellname)
	end
end

rn.player.clear = function()
	rn.data_store():remove_all_of("player.")
	rn.data_store():remove_all_of("player_slot.")
	-- remove all equipment from player and reset its health.
	local player = rn.player.get()
	if player == nil or not rn.current_scene():contains_entity(player) then return end
	rn.item.foreach_equipped(player, function(slot, item)
		rn.item.unequip(player, slot)
	end)
	rn.entity.prefabs.combat_stats.full_heal(player)
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
	local hp_lost = rn.data_store():read("player.hp_lost")
	rn.current_scene():entity_write(player, "hp_lost", hp_lost)

	for slot, slotdata in pairs(rn.spell.slot) do
		local spellname = rn.data_store():read("player_slot." .. tostring(slot))
		if spellname ~= nil then
			rn.entity.prefabs.spell_slots.equip_spell(player, spellname)
		end
	end
	rn.data_store():remove_all_of("player.")
end