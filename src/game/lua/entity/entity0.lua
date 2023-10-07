local id = 0
rn.entity.type["player_lady_melistra"] = id

function keep_playing_animation(e, anim_id, loop)
	if e:get_playing_animation_id() ~= anim_id or not e:is_animation_playing() then
		e:play_animation(anim_id, loop)
	end
end

rn.entity_handler[id] =
{
	preinit = function(ent)
		ent:set_name("Lady Melistra")
		ent:set_model(rn.model.humanoid)
	end,
	postinit = function(ent)
		ent:get_element():object_set_texture_tint(2, 0, 1, 0.1, 0.1)
	end,
	update = function(ent)
		local e = ent:get_element()
		local moving = e:is_animation_playing() and e:get_playing_animation_id() == 8
		local movement_speed = 3.0

		if tz.window():is_key_down("a") then
			x, y = e:get_position()
			x = x - movement_speed * rn.delta_time
			e:set_position(x, y)
			e:face_left()
			moving = true
		elseif tz.window():is_key_down("d") then
			x, y = e:get_position()
			x = x + movement_speed * rn.delta_time
			e:set_position(x, y)
			e:face_right()
			moving = true
		else
			if e:get_playing_animation_id() == 8 then
				e:skip_animation()
				moving = false
			end
		end

		if moving then
			keep_playing_animation(e, 8, false)
		elseif tz.window():is_key_down("z") then
			e:play_animation(6, false)
			e:queue_animation(2, false)
		else
			keep_playing_animation(e, 6, false)
		end
	end
}