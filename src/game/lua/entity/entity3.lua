local id = 3
local typestr = "mannequin"
-- intended to be used to display dropped items on the floor.
-- for each dropped item, you can equip it onto this mannequin and it will display as if being worn by an invisible humanoid.
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	preinit = function(ent)
		ent:set_name("Invisible Mannequin")
		ent:set_model(rn.model.humanoid)
	end,
	postinit = function(ent)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.3)
		ent:get_element():object_set_visibility(2, false)
		ent:get_element():face_forward()
	end
}