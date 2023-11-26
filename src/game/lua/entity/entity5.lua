local id = 5
local typestr = "melee swing area"

rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()

	end,
	preinit = function(ent)
		ent:set_name("Melee Swing Area")
		ent:set_model(rn.model.quad)
	end,
	postinit = function(ent)
		ent:get_element():object_set_visibility(2, false)
		ent:get_element():set_uniform_scale(ent:get_element():get_uniform_scale() * 2.5)
		rn.entity_data_write(ent, "impl.targetable", false, "impl.projectile_skip", true, "duration_remaining", 1.0, "hit_enemy_count", 0)
	end,
	update = function(ent)
		-- for each enemy colliding with us, hit them for damage.
		local owner_uid, duration_remaining, hit_enemy_count = rn.entity_data_read(ent, "owner", "duration_remaining", "hit_enemy_count")
		tz.assert(owner_uid ~= nil)
		local owner = rn.scene():get_uid(owner_uid)
		tz.assert(owner ~= nil)

		local hit_enemies = {}
		for i=1,hit_enemy_count,1 do
			local cur = rn.entity_data_read(ent, "hit_enemy." .. i)	
			hit_enemies[i] = cur
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

			if not found and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_data_read(ent2, "impl.targetable") ~= false then
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = owner:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_attack_power()
				rn.combat.process_event(evt)
				hit_enemy_count = hit_enemy_count + 1
				hit_enemies[hit_enemy_count] = ent2:uid()
				rn.entity_data_write(ent, "hit_enemy." .. string.format("%.0f", hit_enemy_count), ent2:uid(), "hit_enemy_count", hit_enemy_count)
			end
		end)

		duration_remaining = duration_remaining - rn.delta_time
		if duration_remaining < 0.0 then
			rn.scene():remove_uid(ent:uid())
		end
		rn.entity_data_write(ent, "duration_remaining", duration_remaining)
	end
}