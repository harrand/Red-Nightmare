rn.mods.basegame.buffs.spell_stolen_to =
{
	name = "Spell has been stolen.",
	duration = 10.0,
	internal = false,
	on_advance = function(uuid, delta_seconds)
		local spellname = rn.current_scene():entity_read(uuid, "stolen_spell")
		local applied = rn.current_scene():entity_read(uuid, "spellsteal_applied")
		if spellname == nil or applied == true then return end
		-- now here's a problem
		-- spellsteal is of a certain slot. the spell we stole may not be that.
		-- so we cant just equip the spell. we must force-equip it to a specific slot.
		local slot = rn.spell.spells.spellsteal.slot
		rn.entity.prefabs.spell_slots.equip_spell(uuid, spellname, true, slot)
		rn.current_scene():entity_write(uuid, "spellsteal_applied", true)
	end,
	on_remove = function(uuid)
		-- equip spellsteal back.
		rn.entity.prefabs.spell_slots.equip_spell(uuid, "spellsteal", true)
		rn.current_scene():entity_write(uuid, "spellsteal_applied", false)
	end,
	on_cast_success = function(uuid, spellname, castx, casty)
		local stolen_spell = rn.current_scene():entity_read(uuid, "stolen_spell")
		if spellname == stolen_spell then
			rn.buff.remove(uuid, "spell_stolen_to")
		end
	end
}