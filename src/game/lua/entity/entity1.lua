local id = 1
local typestr = "fireball"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		for i=0,3,1 do
			tm:register_texture(typestr .. ".sprite" .. i, "./res/images/sprites/magic_ball/magic_ball" .. i .. ".png")
		end
	end,
	preinit = function(ent)
		ent:set_name("Fireball")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			shoot_dir = nil,
			spawned_at = tz.time(),
			collided_this_update = false
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(1, 0, texh)
		ent:get_element():object_set_texture_tint(1, 0, 1.0, 0.35, 0.05)
		ent:get_element():face_right()
		ent:get_element():rotate(-1.5708)
		local stats = ent:get_base_stats()
		stats:set_movement_speed(stats:get_movement_speed() * 2.0)
		ent:set_base_stats(stats)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		data.flipbook_timer = data.flipbook_timer + rn.delta_time
		-- when flipbook timer hits a threshold (fps / 4), advance to the next frame
		if data.flipbook_timer > 0.1 then
			data.flipbook_timer = 0
			data.cur_texture_id = (data.cur_texture_id + 1) % 4
			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. data.cur_texture_id)
			ent:get_element():object_set_texture_handle(1, 0, texh)
		end
		local x, y = ent:get_element():get_position()

		tz.assert(data.shoot_dir ~= nil)
		local movement_speed = ent:get_stats():get_movement_speed()
		if data.shoot_dir == "right" then
			x = x + movement_speed * rn.delta_time
		elseif data.shoot_dir == "left" then
			x = x - movement_speed * rn.delta_time
		elseif data.shoot_dir == "forward" then
			y = y - movement_speed * rn.delta_time
		elseif data.shoot_dir == "backward" then
			y = y + movement_speed * rn.delta_time
		else
			tz.assert(false)
		end

		ent:get_element():set_position(x, y)

		-- we only live for 5 seconds
		if data.spawned_at + 5000 <= tz.time() then
			-- WE DIE NOW :)
			rn.scene():remove_uid(ent:uid())
		end

		rn.for_each_collision(ent, function(ent2)
			if not data.collided_this_update and rn.get_relationship(ent, ent2) == "hostile" then
				data.collided_this_update = true
				ent2:get_element():play_animation(7, false)
			end
		end)

		if data.collided_this_update then
			rn.scene():remove_uid(ent:uid())
		end
	end
}