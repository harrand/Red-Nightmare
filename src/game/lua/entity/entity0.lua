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
		tracy.ZoneBeginN(typestr .. " - static init")
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity0.png")
		tracy.ZoneEnd()
	end,
	preinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - preinit")
		ent:set_name("Lady Melistra")
		ent:set_model(rn.model.humanoid)
		rn.entity.data[ent:uid()] =
		{
			cast_id = nil,
			cast_begin = nil,
			face_dir = "forward"
		}
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		tracy.ZoneEnd()
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		tz.assert(ent:get_name() == "Lady Melistra")
		tracy.ZoneBeginN(typestr .. " - update")
		tracy.ZoneBeginN("get element")
		local e = ent:get_element()
		tracy.ZoneEnd()
		tracy.ZoneBeginN("get is moving")
		local moving = e:is_animation_playing() and e:get_playing_animation_id() == 8
		tracy.ZoneEnd()

		local xdiff = 0
		local ydiff = 0
		tracy.ZoneBeginN("get window")
		local wnd = tz.window()
		tracy.ZoneEnd()

		tracy.ZoneBeginN("input handling")
		if rn.is_key_down("w") then
			ydiff = ydiff + 1
			e:face_backward()
			data.face_dir = "backward"
		end
		if rn.is_key_down("s") then
			ydiff = ydiff - 1
			e:face_forward()
			data.face_dir = "forward"
		end
		if rn.is_key_down("a") then
			xdiff = xdiff - 1
			e:face_left()
			data.face_dir = "left"
		end
		if rn.is_key_down("d") then
			xdiff = xdiff + 1
			e:face_right()
			data.face_dir = "right"
		end
		if rn.is_key_down("esc") and data.cast_begin ~= nil then
			data.cast_begin = nil
			print("cancelcast")
		end
		tracy.ZoneEnd()

		if (xdiff ~= 0 or ydiff ~= 0) and data.cast_begin == nil then
			tracy.ZoneBeginN("movement")
			moving = true
			local x, y = e:get_position()
			local hypot = math.sqrt(xdiff*xdiff + ydiff*ydiff)
			xdiff = xdiff / hypot
			ydiff = ydiff / hypot
			x = x + xdiff * ent:get_movement_speed() * rn.delta_time
			y = y + ydiff * ent:get_movement_speed() * rn.delta_time
			e:set_position(x, y)
			e:set_animation_speed(math.sqrt(ent:get_movement_speed() / 3.0))
			tracy.ZoneEnd()
		else
			tracy.ZoneBeginN("stationary")
			e:set_animation_speed(1.0)
			if e:get_playing_animation_id() == 8 then
				print("POO")
				e:skip_animation()
				moving = false
			end
			tracy.ZoneEnd()
		end

		local cast_times = 
		{
			800, 1200
		}

		if data.cast_begin ~= nil then
			-- we are casting.
			local cast_end = data.cast_begin + cast_times[data.cast_id + 1]
			if tz.time() >= cast_end then
				print("cast end!")
				local sc = rn.scene()
				local projectile = sc:get(sc:add(1))
				local x, y = e:get_subobject_position(17)
				if data.cast_id == 1 then
					local x2, y2 = e:get_subobject_position(13)
					x = (x + x2) / 2.0
					y = (y + y2) / 2.0
					projectile:get_element():set_uniform_scale(projectile:get_element():get_uniform_scale() * 2)
				end
				projectile:get_element():set_position(x, y)
				data.cast_begin = nil
				local projdata = rn.entity.data[projectile:uid()]
				projdata.shoot_dir = data.face_dir
				if data.face_dir == "right" then
					-- do nothing. sprite faces right by default
					print("shoot right")
				elseif data.face_dir == "left" then
					print("shoot left")
					projectile:get_element():rotate(3.14159)
				elseif data.face_dir == "forward" then
					print("shoot forward")
					projectile:get_element():rotate(-1.5708)
				elseif data.face_dir == "backward" then
					print("shoot backward")
					projectile:get_element():rotate(1.5708)
				else
					tz.assert(false)
				end
			end
		else
			if moving then
				keep_playing_animation(e, 8, false)
			elseif wnd:is_mouse_down("left") then
				data.cast_begin = tz.time()
				data.cast_id = 0
				--e:play_animation(6, false)
				--e:queue_animation(2, false)
				e:play_animation(2, false)
			elseif wnd:is_mouse_down("right") then
				data.cast_begin = tz.time()
				data.cast_id = 1
				e:play_animation(5, false)
			else
				keep_playing_animation(e, 6, false)
			end
		end
		tracy.ZoneEnd()
	end
}