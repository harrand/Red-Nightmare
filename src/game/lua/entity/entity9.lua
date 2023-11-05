local id = 9
local typestr = "sprite obstacle"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		--tm:register_texture("grassy.background", "./res/images/scenery/backgrounds/background_grassy.png")
	end,
	preinit = function(ent)
		ent:set_collideable(true)
		ent:set_immoveable(true)
		ent:set_model(rn.model.quad)
		ent:set_faction(rn.faction_id.pure_enemy)
		local stats = ent:get_base_stats()
		stats:set_maximum_health(1)
		ent:set_base_stats(stats)
	end,
	postinit = function(ent)
		ent:set_name("Obstacle")
		ent:get_element():object_set_texture_tint(2, 0, 1, 1, 0);
	end,
	update = function(ent)
	end
}
