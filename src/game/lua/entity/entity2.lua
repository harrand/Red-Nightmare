local id = 2
local typestr = "enemy_melistra_zombie"
rn.entity.type[typestr] = id

rn.entity_handler[id] =
{
	static_init = function()
		tracy.ZoneBeginN(typestr .. " - static init")
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity2.png")
		rn.texture_manager():register_texture(typestr .. ".helm", "./res/images/skins/helm/med_helm.png")
		rn.texture_manager():register_texture(typestr .. ".body", "./res/images/skins/body_armour/chainmail.png")
		tracy.ZoneEnd()
	end,
	preinit = function(ent)
		ent:set_name("Zombie (Melistra)")
		ent:set_model(rn.model.humanoid)
		local stats = ent:get_base_stats()
		stats:set_maximum_health(50)
		ent:set_base_stats(stats)
		rn.entity.data[ent:uid()] =
		{

		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(3, 0, texh)
		local bodytexh = rn.texture_manager():get_texture(typestr .. ".body")
		ent:get_element():object_set_texture_handle(5, 0, bodytexh)
		local helmtexh = rn.texture_manager():get_texture(typestr .. ".helm")
		ent:get_element():object_set_texture_handle(7, 0, helmtexh)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)

		ent:get_element():play_animation(12, true)
		ent:set_faction(rn.faction_id.player_enemy)

		local stats = ent:get_base_stats()
		stats:set_movement_speed(1.0)
		ent:set_base_stats(stats)
	end,
	update = function(ent)
		if not ent:is_dead() then
			rn.entity_move{ent = ent, dir = "right", movement_anim_id = 12}
		end
	end
}