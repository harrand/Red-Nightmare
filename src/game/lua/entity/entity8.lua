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

		rn.entity.data[ent:uid()] =
		{
			dynamic_texture_scale = false,
			texture_scale_zoom = 1
		}
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		if data.dynamic_texture_scale then
			ent:get_element():object_set_texture_scale(2, 0, ent:get_element():get_uniform_scale() / data.texture_scale_zoom)
		end
	end
}
