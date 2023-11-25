local id = 8
local typestr = "sprite scenery"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		--tm:register_texture("grassy.background", "./res/images/scenery/backgrounds/background_grassy.png")
	end,
	preinit = function(ent)
		ent:set_collideable(false)
	end,
	postinit = function(ent)
		ent:set_name("Scenery")
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		local dynamic_texture_scale, texture_scale_zoom = rn.entity_data_read(ent, "dynamic_texture_scale", "texture_scale_zoom")
		if dynamic_texture_scale then
			ent:get_element():object_set_texture_scale(2, 0, ent:get_element():get_uniform_scale() / texture_scale_zoom)
			ent:get_element():object_set_texture_scale(2, 1, ent:get_element():get_uniform_scale() / texture_scale_zoom)
		end
	end
}
