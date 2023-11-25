local id = 14
local typestr = "touch_of_death"
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
		ent:set_name("Touch of Death")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			shoot_dir = nil,
			spawned_at = tz.time(),
			collided_this_update = false,
			magic_type = nil
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		local light_id = rn.scene():add_light()
		local light = rn.scene():get_light(light_id)
		light:set_power(2.0)
		rn.entity_data_write(ent, "impl.light", light_id)
	end,
	deinit = function(ent)
		local light_id = rn.entity_data_read(ent, "impl.light")
		tz.assert(light_id ~= nil)
		rn.scene():remove_light(light_id)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		local stats = ent:get_base_stats()
		stats:set_movement_speed(12.0)
		ent:set_base_stats(stats)
		local magic_type, light_id = rn.entity_data_read(ent, "magic_type", "impl.light")
		local r, g, b = rn.damage_type_get_colour(magic_type)
		ent:get_element():object_set_texture_tint(2, 0, r, g, b)
		data.flipbook_timer = data.flipbook_timer + rn.delta_time
		-- when flipbook timer hits a threshold (fps / 4), advance to the next frame
		if data.flipbook_timer > 0.1 then
			data.flipbook_timer = 0
			data.cur_texture_id = (data.cur_texture_id + 1) % 4
			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. data.cur_texture_id)
			ent:get_element():object_set_texture_handle(2, 0, texh)
		end
		local x, y = ent:get_element():get_position()

		local light = rn.scene():get_light(light_id)
		light:set_position(x, y)
		light:set_colour(r, g, b)

		if not data.shoot_direct then
			rn.entity_move({ent = ent, dir = data.shoot_dir, face_in_direction = false})
		else
			rn.entity_move({ent = ent, vecdir_x = data.shoot_vec_x, vecdir_y = data.shoot_vec_y, face_in_direction = false})
		end

		-- we only live for 5 seconds
		if data.spawned_at + 5000 <= tz.time() then
			-- WE DIE NOW :)
			rn.scene():remove_uid(ent:uid())
		end

		rn.for_each_collision(ent, function(ent2)
			local projectile_skip, undead = rn.entity_data_read(ent2, "impl.projectile_skip", "impl.undead")
			if not data.collided_this_update and ent2:is_valid() and ent2:is_dead() and projectile_skip ~= true and not undead then
				-- bring the dead bloke back to life. set him to our faction.
				data.collided_this_update = true
				if data.owner ~= nil and data.owner:is_valid() then
					ent2:set_faction(data.owner:get_faction())
				end
				-- set his texture tint to be darker.
				if ent2:get_model() == rn.model.humanoid then
					ent2:get_element():object_set_texture_tint(3, 0, rn.damage_type_get_colour(magic_type))
				end
				ent2:set_health(ent2:get_stats():get_maximum_health())

				if ent2:get_type() == 13 then
					-- its an elemental. set its type to our magic type.
					rn.entity_data_write(ent2, "magic_type", magic_type)
					-- turn all its lights back on too!
					for i=1,2,1 do
						local light_id = rn.scene():add_light()
						local light = rn.scene():get_light(light_id)
						light:set_power(0.8)
						rn.entity_data_write(ent2, "impl.lights[" .. i .. "]", light_id)
					end
				else
					-- undead things despawn very fast.
					rn.entity_data_write(ent2, "impl.custom_despawn_timer", 5000, "impl.undead", true)
				end
			end
		end)

		if data.collided_this_update then
			rn.scene():remove_uid(ent:uid())
		end
	end
}