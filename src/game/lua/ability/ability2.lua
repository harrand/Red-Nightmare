local id = 2
local typestr = "Melee"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 650,
	magic_type = "Physical",
	cast_type = rn.cast.type.melee_1h_horizontal,
	on_cast = function(ent)
		local swing = rn.scene():get(rn.scene():add(5))
		swing:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		swing:get_element():set_position(x, y)
		swing:set_base_stats(ent:get_base_stats())
		rn.entity_data_write(swing, "owner", ent:uid())
	end
}