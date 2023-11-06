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

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			spawned_at = tz.time(),
			owner = nil,
		}
	end,
	postinit = function(ent)
		ent:get_element():set_uniform_scale(ent:get_element():get_uniform_scale() * 4)
		local data = rn.entity_get_data(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		data.hit_enemies = {}
	end,
	update = function(ent)
		-- for each enemy colliding with us, hit them for damage.
		local data = rn.entity_get_data(ent)
		local owner = data.owner
		tz.assert(owner ~= nil)
		-- this lasts for multiple frames. we dont want to hit the same person twice.
		-- so each person we hit we add their uid. and if we collide with that uid again dont hit.
		rn.for_each_collision(ent, function(ent2)
			if not data.hit_enemies[ent2:uid()] and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" then
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = owner:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power()
				rn.combat.process_event(evt)
				data.hit_enemies[ent2:uid()] = true

				-- buff owner's speed
				local speed_buff = rn.new_buff()
				speed_buff:set_time_remaining(3.0)
				speed_buff:set_increased_movement_speed(30)
				owner:apply_buff(speed_buff)
			end
		end)

		data.flipbook_timer = data.flipbook_timer + rn.delta_time
		if data.flipbook_timer > 0.1 then
			data.flipbook_timer = 0
			data.cur_texture_id = data.cur_texture_id + 1

			if data.cur_texture_id > 6 then
				rn.scene():remove_uid(ent:uid())
				return
			end

			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. data.cur_texture_id)
			ent:get_element():object_set_texture_handle(2, 0, texh)
		end
	end
}