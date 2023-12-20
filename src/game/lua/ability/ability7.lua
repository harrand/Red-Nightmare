local id = 7
local typestr = "Fiery Detonation"
rn.ability.type[typestr] = id

rn.abilities[id] = 
{
	name = typestr,
	base_cast_time = 1250,
	magic_type = "Fire",
	cast_type = rn.cast.type.spell_2h_omni,
	dual_wield_cast = true,
	on_cast = function(ent)
		local nova = rn.scene():get(rn.scene():add(10))
		nova:set_faction(rn.faction_id.pure_enemy)
		-- spellpower equal to current health (aka ent kills itself too)
		local bstats = nova:get_base_stats()
		bstats:set_spell_power(ent:get_health() + ent:get_stats():get_spell_power())
		nova:set_base_stats(bstats)
		local x, y = ent:get_element():get_subobject_position(21)
		local x2, y2 = ent:get_element():get_subobject_position(17)
		x = (x + x2) / 2.0
		y = (y + y2) / 2.0
		nova:get_element():set_position(x, y)
		rn.play_sound("fire_nova.mp3")
	end
}