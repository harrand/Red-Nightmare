rn = rn or {}

rn.static_init = function()
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Static Init")

	for prefabname, prefabdata in pairs(rn.entity.prefabs) do
		if prefabdata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(prefabname .. " - Static Init")
			prefabdata.static_init()
		end
	end

	for levelname, leveldata in pairs(rn.level.levels) do
		if leveldata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(levelname .. " - Static Init")
			leveldata.static_init()
		end
	end

	for spellname, spelldata in pairs(rn.spell.spells) do
		if spelldata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(spellname .. " - Static Init")
			spelldata.static_init()
		end
	end

	for itemname, itemdata in pairs(rn.item.items) do
		if itemdata.static_init ~= nil then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name(itemname .. " - Static Init")
			itemdata.static_init()
		end
	end
	
	rn_impl_static_armour_load()
end

rn.update = function(delta_seconds)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("rn.update(...)")
	-- global update. this is invoked *once* per frame, on the main thread (although you should assume the thread is unspecified)
	-- entity updates don't happen here - although they may still be in progress when this is called.
	-- global-state updates should happen here, such as current-level-advance.
	rn.level.current_level_update(delta_seconds)
	rn.camera_follow_player(delta_seconds)
	rn.display_player_spell_slots()
end

rn.camera_follow_player = function(delta_seconds)
	local player_uuid = rn.level.data_read("player")
	if player_uuid == nil or not rn.current_scene():contains_entity(player_uuid) then return end

	local camx, camy = rn.renderer():get_camera_position()
	local px, py, pz = rn.current_scene():entity_get_global_position(player_uuid, rn.entity.prefabs.bipedal.head)
	rn.renderer():set_camera_position(px, py + 12, pz + 15)
	rn.renderer():set_camera_rotation(-0.8, 0, 0)
end

-- display_player_spell_slots is good at making the text if it isnt already there, but very bad at updating the text.
-- to get around this, whenever the player equips a new spell, it just calls this function instead and we go ahead and update the text. bit of a hack, but hey hum
rn.player_spell_slot_override = function(spellname, slot_override)
	local slot = slot_override or rn.spell.spells[spellname].slot
	local slotdata = rn.spell.slot[slot]
	local cur_slot_text = rn.level.data_read("player_spell_slot_" .. tostring(slot))
	if cur_slot_text ~= nil then
		rn.renderer():remove_string(cur_slot_text)
		cur_slot_text = rn.renderer():add_string(0, 0, 8, slotdata.default_keybind .. ": " .. spellname, slotdata.colour[1] * 2, slotdata.colour[2] * 2, slotdata.colour[3] * 2)
		rn.level.data_write("player_spell_slot_" .. tostring(slot), cur_slot_text)
	end
end

rn.display_player_spell_slots = function()
	local player_uuid = rn.level.data_read("player")
	if player_uuid == nil or not rn.current_scene():contains_entity(player_uuid) then return end

	local w, h = tz.window():get_dimensions()

	for slot, slotdata in pairs(rn.spell.slot) do
		local spellname = rn.entity.prefabs.spell_slots.get_spell(player_uuid, slot)
		if spellname ~= nil then
			-- we need a piece of text.
			local xpos = (w / 2) + (slotdata.impl_id * 250.0)
			local ypos = 60
			-- ypos increases by the spell cooldown (causing it to slide upwards until its off-cd)
			local cd = rn.entity.prefabs.spell_slots.get_spell_cooldown(player_uuid, slot) or 0.0
			local cd_progress = cd / (rn.spell.spells[spellname].cooldown or 1.0)
			ypos = ypos + (-cd_progress * 100)
			local cur_slot_text = rn.level.data_read("player_spell_slot_" .. tostring(slot))
			if cur_slot_text == nil then
				cur_slot_text = rn.renderer():add_string(xpos, ypos, 8, slotdata.default_keybind .. ": " .. spellname, slotdata.colour[1] * 2, slotdata.colour[2] * 2, slotdata.colour[3] * 2)
				rn.level.data_write("player_spell_slot_" .. tostring(slot), cur_slot_text)
			else
				rn.renderer():string_set_position(cur_slot_text, xpos, ypos)
			end
		end
	end
end