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
			magic_type = nil
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		local light_id = rn.scene():add_light();
		local light = rn.scene():get_light(light_id)
		light:set_power(2.0)
		rn.entity_data_write(ent, "impl.targetable", false, "impl.light", light_id)
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
		local magic_type = rn.entity_data_read(ent, "magic_type")
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

		local light_id, shoot_direct = rn.entity_data_read(ent, "impl.light", "shoot_direct")
		local light = rn.scene():get_light(light_id)
		light:set_position(x, y)
		light:set_colour(r, g, b)

		if not shoot_direct then
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
			if ent2:is_valid() and ent2:get_type() == ent:get_type() then
				-- if two fireballs collide:
				-- find the midpoint between the two fireballs
				local mx, my = ent:get_element():get_position()
				local m2x, m2y = ent2:get_element():get_position()
				mx = (mx + m2x) * 0.5
				my = (my + m2y) * 0.5
				-- spawn a large explosion there that is dangerous to everyone
				local explosion = rn.scene():get(rn.scene():add(10))
				local explosiondata = rn.entity_get_data(explosion)
				explosion:set_base_stats(ent:get_stats())
				explosion:set_faction(rn.faction_id.pure_enemy)
				explosion:get_element():set_position(mx, my)
				-- no owner!
				explosiondata.owner = nil
				-- todo: set magic_type of explosiondata (currently only supports fire)
				-- despawn both fireballs
				rn.scene():remove_uid(ent:uid())
				rn.scene():remove_uid(ent2:uid())
				return false
			end
			local target_projectile_skip = rn.entity_data_read(ent2, "impl.projectile_skip")
			if not data.collided_this_update and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and target_projectile_skip ~= true then
				data.collided_this_update = true
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = data.owner:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power()
				evt.damage_type = magic_type
				rn.combat.process_event(evt)

				if data.owner ~= nil and data.owner:is_valid() and data.owner:get_type() == 0 then
					-- powerup: dire fireball (on by default)
					-- each unique enemy you hit with fireball increases your spellpower by 20% for 10 seconds.
					local spbuff = rn.new_buff()
					spbuff:set_name("Dire Fireball" .. ent2:uid())
					spbuff:set_time_remaining(10)
					spbuff:set_amplified_spell_power(1.2)
					data.owner:apply_buff(spbuff)
				end
			end
		end)

		if data.collided_this_update then
			rn.scene():remove_uid(ent:uid())
		end
	end
}