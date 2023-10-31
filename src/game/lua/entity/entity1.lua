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
			collided_this_update = false,
			colour_r,
			colour_g,
			colour_b
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		ent:get_element():face_right()
		ent:get_element():rotate(-1.5708)
		local stats = ent:get_base_stats()
		stats:set_movement_speed(stats:get_movement_speed() * 2.0)
		ent:set_base_stats(stats)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		ent:get_element():object_set_texture_tint(2, 0, data.colour_r, data.colour_g, data.colour_b)
		data.flipbook_timer = data.flipbook_timer + rn.delta_time
		-- when flipbook timer hits a threshold (fps / 4), advance to the next frame
		if data.flipbook_timer > 0.1 then
			data.flipbook_timer = 0
			data.cur_texture_id = (data.cur_texture_id + 1) % 4
			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. data.cur_texture_id)
			ent:get_element():object_set_texture_handle(2, 0, texh)
		end
		local x, y = ent:get_element():get_position()

		if not data.shoot_direct then
			rn.entity_move({ent = ent, dir = data.shoot_dir, face_in_direction = false})
		else
			rn.entity_move({ent = ent, vecdir_x = data.shoot_vec_x, vecdir_y = data.shoot_vec_y, face_in_direction = false})
		end

		-- we only live for 5 seconds
		if data.spawned_at + 5000 <= tz.time() then
			-- WE DIE NOW :)
			print("FIREBALL GO DEATH BYE BYE")
			rn.scene():remove_uid(ent:uid())
		end

		rn.for_each_collision(ent, function(ent2)
			if not data.collided_this_update and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" then
				data.collided_this_update = true
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = ent:uid()
				evt.damagee = ent2:uid()
				evt.value = 20
				rn.combat.process_event(evt)
			end
		end)

		if data.collided_this_update then
			print("FIREBALL GO DEATH BYE BYE")
			rn.scene():remove_uid(ent:uid())
		end
	end
}