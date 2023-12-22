local id = 17
local typestr = "consecrated area"

rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		rn.texture_manager():register_texture(typestr .. ".sprite", "./res/images/sprites/consecrate.png")
	end,
	preinit = function(ent)
		ent:set_name("Consecration")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			owner = nil,
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		ent:get_element():set_uniform_scale(0.0)
		local data = rn.entity_get_data(ent)
		-- live for 0.5 seconds
		data.duration = 12.0
		data.duration_remaining = data.duration
		data.tick_rate = 0.5
		data.tick_duration = data.tick_rate
		data.hit_enemies = {}
		data.impl.projectile_skip = true
		data.impl.targetable = false

		data.impl.light = rn.scene():add_light();
		data.impl.targetable = false
	end,
	deinit = function(ent)
		local data = rn.entity_get_data(ent)
		tz.assert(data.impl.light ~= nil)
		rn.scene():remove_light(data.impl.light)
	end,
	update = function(ent)
		-- for each enemy colliding with us, hit them for damage.
		local data = rn.entity_get_data(ent)
		local r, g, b = rn.damage_type_get_colour(data.magic_type)
		ent:get_element():object_set_texture_tint(2, 0, r, g, b)

		local owner = data.owner
		tz.assert(owner ~= nil)
		-- this lasts for multiple frames. we dont want to hit the same person twice.
		-- so each person we hit we add their uid. and if we collide with that uid again dont hit.
		rn.for_each_collision(ent, function(ent2)
			if not data.hit_enemies[ent2:uid()] and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_get_data(ent2).impl.targetable ~= false then
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = owner:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power() * 0.2
				evt.damage_type = data.magic_type
				rn.combat.process_event(evt)
				data.hit_enemies[ent2:uid()] = true
				rn.play_sound("melee.mp3", 0.4)
			end
		end)

		ent:get_element():face_forward2d()
		ent:get_element():rotate(data.duration_remaining / 5.0)

		data.duration_remaining = data.duration_remaining - rn.delta_time
		data.tick_duration = data.tick_duration - rn.delta_time
		if data.tick_duration < 0.0 then
			data.hit_enemies = {}
			data.tick_duration = data.tick_rate
		end
		if data.duration_remaining < 1.0 then
			ent:get_element():set_uniform_scale(data.duration_remaining * 2.5)
		end
		if data.duration_remaining > (data.duration - 1.0) then
			ent:get_element():set_uniform_scale((data.duration - data.duration_remaining) * 2.5)
		end

		local x, y = ent:get_element():get_position()

		data.impl.light:set_position(x, y)
		data.impl.light:set_power(ent:get_element():get_uniform_scale() * 2)
		data.impl.light:set_colour(r, g, b)

		if data.duration_remaining < 0.0 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}