local id = 0
local typestr = "Heal"
rn.ability.type[typestr] = id
-- entity heals itself for 100% of spell power

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1000,
	magic_colour_r = 1.0,
	magic_colour_g = 0.85,
	magic_colour_b = 0.4,
	cast_type = rn.cast.type.spell_1h_omni,
	on_cast = function(ent)
		local evt = rn.entity_heal_entity_event:new()
		evt.healer = ent:uid()
		evt.healee = ent:uid()
		evt.value = ent:get_stats().get_spell_power()
		rn.combat.process_event(evt)
	end
}