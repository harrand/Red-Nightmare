local id = 0
local typestr = "Heal"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	base_cast_time = 1000,
	magic_colour_r = 1.0,
	magic_colour_g = 1.0,
	magic_colour_b = 0.4,
	on_cast = function(ent)
		local evt = rn.entity_heal_entity_event:new()
		evt.healer = ent:uid()
		evt.healee = ent:uid()
		evt.value = 50
		rn.combat.process_event(evt)
	end
}