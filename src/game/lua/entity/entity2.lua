local id = 2
local typestr = "enemy_melistra_zombie"
rn.entity.type[typestr] = id

rn.entity_handler[id] =
{
	static_init = function()
		tracy.ZoneBeginN(typestr .. " - static init")
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity2.png")
		rn.texture_manager():register_texture(typestr .. ".helm", "./res/images/skins/helm/med_helm.png")
		tracy.ZoneEnd()
	end,
	preinit = function(ent)
		ent:set_name("Zombie (Melistra)")
		ent:set_model(rn.model.humanoid)
		rn.entity.data[ent:uid()] =
		{

		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		local helmtexh = rn.texture_manager():get_texture(typestr .. ".helm")
		ent:get_element():object_set_texture_handle(6, 0, helmtexh)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)

		ent:get_element():play_animation(12, true)
	end,
	update = function(ent)

	end
}