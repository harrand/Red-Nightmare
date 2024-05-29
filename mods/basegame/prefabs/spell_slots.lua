rn.mods.basegame.prefabs.spell_slots =
{
	description = "Entity may have a spell bound to each spell slot",
	equip_spell = function(uuid, spellname)
		local slot = rn.spell.spells[spellname].slot
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