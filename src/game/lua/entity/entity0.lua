local id = 0
local typestr = "player_lady_melistra"
rn.entity.type[typestr] = id

function keep_playing_animation(e, anim_id, loop)
	if e:get_playing_animation_id() ~= anim_id or not e:is_animation_playing() then
		e:play_animation(anim_id, loop)
	end
end

rn.entity_handler[id] =
{
	-- invoked exactly once during game initialisation.
	-- if this entity has any unique bespoke resources to pre-load, now is the time.
	static_init = function()
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity0.png")
	end,
	preinit = function(ent)
		ent:set_name("Lady Melistra")
		ent:set_model(rn.model.humanoid)
	end,
	postinit = function(ent)
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(2, 0, texh)
	end,
	update = function(ent)
		local e = ent:get_element()
		local moving = e:is_animation_playing() and e:get_playing_animation_id() == 8

		local xdiff = 0
		local ydiff = 0

		if tz.window():is_key_down("w") then
			ydiff = ydiff + 1
			e:face_backward()
		end
		if tz.window():is_key_down("s") then
			ydiff = ydiff - 1
			e:face_forward()
		end
		if tz.window():is_key_down("a") then
			xdiff = xdiff - 1
			e:face_left()
		end
		if tz.window():is_key_down("d") then
			xdiff = xdiff + 1
			e:face_right()
		end

		if xdiff ~= 0 or ydiff ~= 0 then
			moving = true
			local x, y = e:get_position()
			local hypot = math.sqrt(xdiff*xdiff + ydiff*ydiff)
			xdiff = xdiff / hypot
			ydiff = ydiff / hypot
			x = x + xdiff * ent:get_movement_speed() * rn.delta_time
			y = y + ydiff * ent:get_movement_speed() * rn.delta_time
			e:set_position(x, y)
			e:set_animation_speed(math.sqrt(ent:get_movement_speed() / 3.0))
		else
			e:set_animation_speed(1.0)
			if e:get_playing_animation_id() == 8 then
				e:skip_animation()
				moving = false
			end
		end

		if moving then
			keep_playing_animation(e, 8, false)
		elseif tz.window():is_mouse_down("left") then
			e:play_animation(6, false)
			e:queue_animation(2, false)
		else
			keep_playing_animation(e, 6, false)
		end
	end
}