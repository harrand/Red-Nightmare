local id = 9
local typestr = "sprite obstacle"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		--tm:register_texture("grassy.background", "./res/images/scenery/backgrounds/background_grassy.png")
		tm:register_texture("scenery.tree0", "./res/images/scenery/foliage/tree0.png")
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
	on_death = function(ent)
		ent:get_element():object_set_visibility(2, false)
	end,
	postinit = function(ent)
		ent:set_name("Obstacle")
		ent:get_element():object_set_texture_tint(2, 0, 1, 1, 1);
	end
}
