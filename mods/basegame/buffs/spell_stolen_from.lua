rn.mods.basegame.buffs.spell_stolen_from =
{
	name = "Your next spell will be stolen.",
	duration = 10.0,
	internal = false,
	on_apply = function(uuid)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r * 0.5, g * 0.5, b * 0.5)
	end,
	on_remove = function(uuid)
		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		rn.entity.prefabs.bipedal.set_colour(uuid, r * 2, g * 2, b * 2)
	end,
	on_cast_success = function(uuid, spellname, castx, casty)
		local thief = rn.current_scene():entity_read(uuid, "spell_thief")
		tz.report("entity " .. uuid .. " has had spell " .. spellname .. " stolen" .. " by " .. thief)
		rn.buff.apply(thief, "spell_stolen_to")
		rn.current_scene():entity_write(thief, "stolen_spell", spellname)
		rn.buff.remove(uuid, "spell_stolen_from")
	end
}