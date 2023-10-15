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
			face_dir = "forward",
			counter = 0,
			cast_effect_right = nil
		}

		local bstats = ent:get_base_stats()
		bstats:set_maximum_health(100)
		ent:set_base_stats(bstats)
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		rn.equip(ent, "Iron Sallet")

		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		ent:set_faction(rn.faction_id.player_ally)

		tracy.ZoneEnd()
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		data.counter = data.counter + rn.delta_time * 2.8957
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
			if data.cast_effect_right ~= nil then
				rn.scene():remove_uid(data.cast_effect_right:uid())
				data.cast_effect_right = nil	
			end
			data.cast_begin = nil
		end
		tracy.ZoneEnd()

		if (xdiff ~= 0 or ydiff ~= 0) and data.cast_begin == nil then
			tracy.ZoneBeginN("movement")
			moving = true
			local x, y = e:get_position()
			local hypot = math.sqrt(xdiff*xdiff + ydiff*ydiff)
			xdiff = xdiff / hypot
			ydiff = ydiff / hypot
			local stats = ent:get_stats()
			local movement_speed = stats:get_movement_speed()
			x = x + xdiff * movement_speed * rn.delta_time
			y = y + ydiff * movement_speed * rn.delta_time
			e:set_position(x, y)
			e:set_animation_speed(math.sqrt(movement_speed / 3.0))
			tracy.ZoneEnd()
		else
			tracy.ZoneBeginN("stationary")
			e:set_animation_speed(1.0)
			if e:get_playing_animation_id() == 8 then
				e:skip_animation()
				moving = false
			end
			tracy.ZoneEnd()
		end

		local cast_times = 
		{
			850, 1100
		}

		if data.cast_begin ~= nil then
			-- we are casting.
			local cast_end = data.cast_begin + cast_times[data.cast_id + 1]
			if tz.time() >= cast_end then
				local sc = rn.scene()
				local projectile = sc:get(sc:add(1))
				projectile:set_faction(ent:get_faction())
				print(projectile:get_name() .. " spawned with faction " .. rn.get_faction(projectile))
				if data.cast_effect_right ~= nil then
					rn.scene():remove_uid(data.cast_effect_right:uid())
					data.cast_effect_right = nil
				end
				if data.cast_effect_left ~= nil then
					rn.scene():remove_uid(data.cast_effect_left:uid())
					data.cast_effect_left = nil
				end
				local x, y = e:get_subobject_position(21)
				if data.cast_id == 1 then
					local x2, y2 = e:get_subobject_position(17)
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
				elseif data.face_dir == "left" then
					projectile:get_element():rotate(3.14159)
				elseif data.face_dir == "forward" then
					projectile:get_element():rotate(-1.5708)
				elseif data.face_dir == "backward" then
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
				data.cast_effect_right = rn.scene():get(rn.scene():add(4))
				rn.entity.data[data.cast_effect_right:uid()].target_entity = ent
				rn.entity.data[data.cast_effect_right:uid()].subobject = 21
				rn.entity.data[data.cast_effect_right:uid()].cast_duration = cast_times[1]
				--e:play_animation(6, false)
				--e:queue_animation(2, false)
				e:play_animation(2, false)
			elseif wnd:is_mouse_down("right") then
				data.cast_begin = tz.time()
				data.cast_id = 1
				data.cast_effect_right = rn.scene():get(rn.scene():add(4))
				rn.entity.data[data.cast_effect_right:uid()].target_entity = ent
				rn.entity.data[data.cast_effect_right:uid()].subobject = 21
				rn.entity.data[data.cast_effect_right:uid()].cast_duration = cast_times[2]
				data.cast_effect_left = rn.scene():get(rn.scene():add(4))
				rn.entity.data[data.cast_effect_left:uid()].target_entity = ent
				rn.entity.data[data.cast_effect_left:uid()].subobject = 17
				rn.entity.data[data.cast_effect_left:uid()].cast_duration = cast_times[2]
				if data.face_dir == "left" or data.face_dir == "right" then
					e:play_animation(4, false)
				else
					e:play_animation(5, false)
				end
			else
				keep_playing_animation(e, 6, false)
			end
		end
		tracy.ZoneEnd()
	end
}