local id = 16
local typestr = "frozen orb"
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
		ent:set_name("Frozen Orb")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			shoot_dir = nil,
			spawned_at = tz.time(),
			collided_this_update = false,
			frostbolt_timer = 0,
			frostbolt_count = 0,
			magic_type = nil
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		ent:get_element():set_uniform_scale(1.7)

		local data = rn.entity_get_data(ent)
		data.impl.light = rn.scene():add_light();
		data.impl.light:set_power(3.2)
		data.impl.targetable = false
	end,
	deinit = function(ent)
		local data = rn.entity_get_data(ent)
		tz.assert(data.impl.light ~= nil)
		rn.scene():remove_light(data.impl.light)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		local stats = ent:get_base_stats()
		stats:set_movement_speed(12.0)
		ent:set_base_stats(stats)
		local r, g, b = rn.damage_type_get_colour(data.magic_type)
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

		data.impl.light:set_position(x, y)
		data.impl.light:set_colour(r, g, b)

		-- frostbolt spawns.
		data.frostbolt_timer = data.frostbolt_timer + rn.delta_time
		local frostbolt_spawnrate = 0.15
		if data.frostbolt_timer >= frostbolt_spawnrate then
			-- spawn a frostbolt
			--rn.cast_spell({ent = ent, ability_name = "Frostbolt", face_cast_direction = true})
			local frostbolt = rn.scene():get(rn.scene():add(1))
			frostbolt:set_faction(ent:get_faction())
			frostbolt:get_element():set_position(ent:get_element():get_position())
			local fbdata = rn.entity_get_data(frostbolt)
			fbdata.shoot_direct = true
			local angular_rate = 0.9
			fbdata.shoot_vec_x = math.sin(data.frostbolt_count * angular_rate)
			fbdata.shoot_vec_y = math.cos(data.frostbolt_count * angular_rate)
			frostbolt:get_element():rotate(math.atan(fbdata.shoot_vec_y, fbdata.shoot_vec_x))
			fbdata.owner = data.owner
			fbdata.self_collide = false

			local bstats = ent:get_stats()
			bstats:set_movement_speed(bstats:get_movement_speed() + 6.0)
			frostbolt:set_base_stats(bstats)
			fbdata.magic_type = "Frost"

			data.frostbolt_timer = 0.0
			data.frostbolt_count = data.frostbolt_count + 1
		end

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
			if not data.collided_this_update and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_get_data(ent2).impl.projectile_skip ~= true then
				data.collided_this_update = true
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = data.owner:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power()
				evt.damage_type = data.magic_type
				rn.combat.process_event(evt)

				rn.play_sound("fireball_hit.mp3")
			end
		end)
	end
}