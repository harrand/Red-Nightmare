local id = 1
rn.entity.type["wall"] = id
rn.entity_handler[id] =
{
	static_init = function()

	end,
	preinit = function(ent)
		ent:set_name("Wall")
		ent:set_model(rn.model.quad)
	end,
	postinit = function(ent)
		ent:get_element():face_right()
	end,
	update = function(ent)
		tz.assert(ent:get_name() == "Wall")
	end
}