local id = 11
local typestr = "Consecrate"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1000,
	magic_type = "Divine",
	cast_type = rn.cast.type.spell_1h_omni,
	on_cast = function(ent)
		local area = rn.scene():get(rn.scene():add(17))
		area:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		area:get_element():set_position(x, y)
		area:set_base_stats(ent:get_stats())
		local areadata = rn.entity_get_data(area)
		areadata.owner = ent
		areadata.magic_type = "Divine"
	end
}