local id = 0
rn.entity.type["player_lady_melistra"] = id
rn.entity_handler[id] =
{
	preinit = function(ent)
		ent:set_name("Lady Melistra")
		ent:set_model(rn.model.humanoid)
	end,
	postinit = function(ent)
		ent:get_element():object_set_texture_tint(3, 0, 1, 0.1, 0.1)
	end,
	update = function(ent)
		if not ent:get_element():is_animation_playing() then
			ent:get_element():play_animation(6, false)
		end
	end
}