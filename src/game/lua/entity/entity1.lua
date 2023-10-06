local id = 1
rn.entity.type["wall"] = id
rn.entity_handler[id] =
{
	preinit = function(ent)
		ent:set_name("Wall")
		ent:set_model(rn.model.quad)
	end,
	postinit = function(ent)

	end
}