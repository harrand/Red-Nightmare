rn.mods.basegame.prefabs.spell_slots =
{
	description = "Entity may have a spell bound to each spell slot",
	equip_spell = function(uuid, spellname)
		local slot = rn.spell.spells[spellname].slot

		-- if we're changing the player's equipped spell, the spell slot display ui needs to be updated:
		if rn.mods.basegame.prefabs.spell_slots.would_replace_on_equip(uuid, spellname) and (uuid == rn.player.get()) then
			rn.player_spell_slot_override(spellname)
		end
		-- if we already have a spell in this slot, drop it
		local existing_spellname = rn.current_scene():entity_read(uuid, "spell_slot." .. tostring(slot))
		rn.current_scene():entity_write(uuid, "spell_slot." .. tostring(slot), spellname);
		if existing_spellname ~= nil then
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
			local spelldrop = rn.spell.drop_at(x, y, existing_spellname)
			rn.entity.prefabs.spell_drop.set_blacklisted(spelldrop, uuid)
		end
	end,
	would_replace_on_equip = function(uuid, spellname)
		local slot = rn.spell.spells[spellname].slot
		return rn.mods.basegame.prefabs.spell_slots.get_spell(uuid, slot) ~= nil
	end,
	get_spell = function(uuid, slot)
		return rn.current_scene():entity_read(uuid, "spell_slot." .. tostring(slot))
	end
}