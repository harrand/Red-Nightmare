local id = 10
local typestr = "allure_of_flame"

rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		for i=0,6,1 do
			tm:register_texture(typestr .. ".sprite" .. i, "./res/images/sprites/fire_explosion/fire_explosion_" .. i .. ".png")
		end
	end,
	preinit = function(ent)
		ent:set_name("Fire Nova")
		ent:set_model(rn.model.quad)
	end,
	postinit = function(ent)
		ent:get_element():set_uniform_scale(ent:get_element():get_uniform_scale() * 4)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		local light_id = rn.scene():add_light()
		local light = rn.scene():get_light(light_id)
		light:set_power(3.5)
		light:set_colour(rn.damage_type_get_colour("Fire"))
		rn.entity_data_write(ent, "impl.light", light_id, "flipbook_timer", 0, "cur_texture_id", 0, "hit_enemy_count", 0)
	end,
	deinit = function(ent)
		local light_id = rn.entity_data_read(ent, "impl.light")
		tz.assert(light_id ~= nil)
		rn.scene():remove_light(light_id)
	end,
	update = function(ent)
		-- for each enemy colliding with us, hit them for damage.
		local flipbook_timer, cur_texture_id, hit_enemy_count = rn.entity_data_read(ent, "flipbook_timer", "cur_texture_id", "hit_enemy_count")

		local hit_enemies = {}
		for i=1,hit_enemy_count,1 do
			local cur = rn.entity_data_read(ent, "hit_enemy." .. i)	
			hit_enemies[i] = cur
		end

		-- the explosion texture is very slightly off-center.
		-- until we fix it, use this dodgy offset.
		local x, y = ent:get_element():get_position()
		x = x + 0.5
		y = y + 0.5
		local light = rn.scene():get_light(rn.entity_data_read(ent, "impl.light"))
		light:set_position(x, y)
		local owner_id = rn.entity_data_read(ent, "owner")
		local owner = nil
		if owner_id ~= nil then
			owner = rn.scene():get_uid(owner_id)
		end
		-- this lasts for multiple frames. we dont want to hit the same person twice.
		-- so each person we hit we add their uid. and if we collide with that uid again dont hit.
		rn.for_each_collision(ent, function(ent2)
			local found = false
			for i=1,hit_enemy_count,1 do
				if hit_enemies[i] == ent2:uid() then
					found = true
				end
			end

			if not found and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_data_read(ent2, "impl.projectile_skip") ~= true then
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = ent:uid()
				if owner ~= nil then
					evt.damager = owner:uid()
				end
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power()
				evt.damage_type = "Fire"
				rn.combat.process_event(evt)
				hit_enemy_count = hit_enemy_count + 1
				hit_enemies[hit_enemy_count] = ent2:uid()
				rn.entity_data_write(ent, "hit_enemy." .. string.format("%.0f", hit_enemy_count), ent2:uid(), "hit_enemy_count", hit_enemy_count)

				-- buff owner's speed
				if owner ~= nil then
					local speed_buff = rn.new_buff()
					speed_buff:set_time_remaining(3.0)
					speed_buff:set_increased_movement_speed(30)
					owner:apply_buff(speed_buff)
				end
			end
		end)

		flipbook_timer = flipbook_timer + rn.delta_time
		if flipbook_timer > 0.1 then
			flipbook_timer = 0
			cur_texture_id = cur_texture_id + 1
			light:set_power(light:get_power() * (5.0/6.0))

			if cur_texture_id > 6 then
				rn.scene():remove_uid(ent:uid())
				return
			end

			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. string.format("%.0f", cur_texture_id))
			ent:get_element():object_set_texture_handle(2, 0, texh)
		end
		rn.entity_data_write(ent, "flipbook_timer", flipbook_timer, "cur_texture_id", cur_texture_id)
	end
}