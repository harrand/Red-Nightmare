local id = 2
local typestr = "Melee"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 650,
	cast_type = rn.cast.type.melee_1h_horizontal,
	on_cast = function(ent)
		local swing = rn.scene():get(rn.scene():add(5))
		swing:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		swing:get_element():set_position(x, y)
		local swingdata = rn.entity_get_data(swing)
		swingdata.owner = ent
		print(ent:get_name() .. " swings their mighty weapon!")
	end
}