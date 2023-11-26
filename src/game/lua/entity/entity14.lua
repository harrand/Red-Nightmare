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
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		local light_id = rn.scene():add_light()
		local light = rn.scene():get_light(light_id)
		light:set_power(2.0)
		rn.entity_data_write(ent, "impl.light", light_id, "flipbook_timer", 0, "cur_texture_id", 0, "spawned_at", tz.time())
	end,
	deinit = function(ent)
		local light_id = rn.entity_data_read(ent, "impl.light")
		tz.assert(light_id ~= nil)
		rn.scene():remove_light(light_id)
	end,
	update = function(ent)
		local stats = ent:get_base_stats()
		stats:set_movement_speed(12.0)
		ent:set_base_stats(stats)
		local magic_type, light_id, shoot_direct, shoot_vec_x, shoot_vec_y, shoot_dir, spawned_at, flipbook_timer, cur_texture_id = rn.entity_data_read(ent, "magic_type", "impl.light", "shoot_direct", "shoot_vec_x", "shoot_vec_y", "shoot_dir", "spawned_at", "flipbook_timer", "cur_texture_id")
		local r, g, b = rn.damage_type_get_colour(magic_type)
		ent:get_element():object_set_texture_tint(2, 0, r, g, b)
		flipbook_timer = flipbook_timer + rn.delta_time
		-- when flipbook timer hits a threshold (fps / 4), advance to the next frame
		if flipbook_timer > 0.1 then
			flipbook_timer = 0
			cur_texture_id = (cur_texture_id + 1) % 4
			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. string.format("%.0f", cur_texture_id))
			ent:get_element():object_set_texture_handle(2, 0, texh)
		end
		local x, y = ent:get_element():get_position()

		local light = rn.scene():get_light(light_id)
		light:set_position(x, y)
		light:set_colour(r, g, b)

		if not shoot_direct then
			rn.entity_move({ent = ent, dir = shoot_dir, face_in_direction = false})
		else
			rn.entity_move({ent = ent, vecdir_x = shoot_vec_x, vecdir_y = shoot_vec_y, face_in_direction = false})
		end

		-- we only live for 5 seconds
		if spawned_at + 5000 <= tz.time() then
			-- WE DIE NOW :)
			rn.scene():remove_uid(ent:uid())
		end
		local collided_this_update = false

		rn.for_each_collision(ent, function(ent2)
			local projectile_skip, undead = rn.entity_data_read(ent2, "impl.projectile_skip", "impl.undead")
			if not collided_this_update and ent2:is_valid() and ent2:is_dead() and projectile_skip ~= true and not undead then
				-- bring the dead bloke back to life. set him to our faction.
				collided_this_update = true
				local owner_id = rn.entity_data_read(ent, "owner")
				if owner_id ~= nil then
					local owner = rn.scene():get_uid(owner_id)
					if owner:is_valid() then
						ent2:set_faction(owner:get_faction())
					end
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

		if collided_this_update then
			rn.scene():remove_uid(ent:uid())
		end
	end
}