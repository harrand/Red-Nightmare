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

		rn.entity.data[ent:uid()] =
		{
			owner = nil,
		}
	end,
	postinit = function(ent)
		ent:get_element():object_set_visibility(1, false)
		local data = rn.entity_get_data(ent)
		-- live for 0.5 seconds
		data.duration_remaining = 1.0
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
				evt.value = 200 -- placeholder. make this equal to owner's attack power
				rn.combat.process_event(evt)
				data.hit_enemies[ent2:uid()] = true
			end
		end)

		data.duration_remaining = data.duration_remaining - rn.delta_time
		if data.duration_remaining < 0.0 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}