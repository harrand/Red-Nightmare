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
		local obj <close> = tz.profzone_obj:new()
		obj:set_name("Scenery Update")
		local data = rn.entity_get_data(ent)
		if data.dynamic_texture_scale then
			local obj2 <close> = tz.profzone_obj:new()
			obj2:set_name("Dynamic Texture Scale Compute")
			local scale = ent:get_element():get_uniform_scale() / data.texture_scale_zoom
			local obj3 <close> = tz.profzone_obj:new()
			obj3:set_name("Dynamic Texture Scale Set")
			ent:get_element():object_set_texture_scale(2, 0, scale)
			ent:get_element():object_set_texture_scale(2, 1, scale)
		end
	end
}
