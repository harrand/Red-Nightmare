rn.mods.basegame.prefabs.spell_slots =
{
	description = "Entity may have a spell bound to each spell slot",
	equip_spell = function(uuid, spellname)
		local slot = rn.spell.spells[spellname].slot

		-- if we're changing the player's equipped spell, the spell slot display ui needs to be updated:
		if rn.mods.basegame.prefabs.spell_slots.would_replace_on_equip(uuid, spellname) and (uuid == rn.player.get()) then
			rn.player_spell_slot_override(spellname)
		end
		rn.current_scene():entity_write(uuid, "spell_slot." .. tostring(slot), spellname);
	end,
	would_replace_on_equip = function(uuid, spellname)
		local slot = rn.spell.spells[spellname].slot
		return rn.mods.basegame.prefabs.spell_slots.get_spell(uuid, slot) ~= nil
	end,
	get_spell = function(uuid, slot)
		return rn.current_scene():entity_read(uuid, "spell_slot." .. tostring(slot))
	end
}