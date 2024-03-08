rn.mods.basegame.prefabs.basic_target_field =
{
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_visible(uuid, false)
	end,
	update = function(uuid, delta_seconds)
		rn.entity.prefabs.sticky.update(uuid, delta_seconds)
	end,
	on_collision = function(me, other)
		local owner = rn.current_scene():entity_read(me, "owner")
		if owner ~= nil and (other ~= owner) then
			local owner_prefab = nil
			local owner_prefab_str = rn.current_scene():entity_read(owner, ".ai")
			if owner_prefab_str ~= nil then
				owner_prefab = rn.entity.prefabs[owner_prefab_str]
			end
			if owner_prefab ~= nil then
				if owner_prefab.on_target_field_collide ~= nil then
					owner_prefab.on_target_field_collide(owner, me, other)
					return false
				end
			end
		end
		return false
	end,
}

rn.mods.basegame.prefabs.zombie_ai =
{
	instantiate = function(uuid)
		rn.current_scene():entity_write(uuid, ".ai", "zombie_ai")
	end,
	get_target = function(uuid)
		return rn.current_scene():entity_read(uuid, "target")
	end,
	set_target = function(uuid, target_uuid)
		rn.current_scene():entity_write(uuid, "target", target_uuid)
	end,
	find_target = function(uuid, target_range)
		local sc = rn.current_scene()
		local target_field = sc:entity_read(uuid, "target_field")
		if target_field ~= nil then
			rn.entity.prefabs.sprite.set_scale(target_field, target_range)
		else
			target_field = sc:add_entity("basic_target_field")
			--rn.entity.prefabs.sprite.set_scale(target_field, target_range)
			sc:entity_write(target_field, "owner", uuid)
			rn.entity.prefabs.sticky.stick_to(target_field, uuid)
			sc:entity_write(uuid, "target_field", target_field)
		end
	end,
	on_target_field_collide = function(uuid, field_uuid, collided_uuid)
		local existing_target = rn.entity.prefabs.zombie_ai.get_target(uuid)
		if existing_target == nil or not rn.current_scene():contains_entity(existing_target) then
			-- need a new target.
			if rn.entity.prefabs.combat_stats.is_alive(collided_uuid) then
				rn.entity.prefabs.zombie_ai.set_target(uuid, collided_uuid)
				rn.current_scene():remove_entity(field_uuid)
				rn.current_scene():entity_write(uuid, "target_field", nil)
			end
		end
	end,
	update = function(uuid, delta_seconds)
		-- if enemy is casting, let them cast.
		if rn.spell.is_casting(uuid) then return end

		local target = rn.entity.prefabs.zombie_ai.get_target(uuid)
		if target ~= nil
		and rn.current_scene():contains_entity(target) then
			if not rn.entity.prefabs.combat_stats.is_alive(target) then
				-- target died. clear target and abort.
				rn.entity.prefabs.zombie_ai.set_target(uuid, nil)
				return
			end
			local tarx, tary = rn.entity.prefabs.sprite.get_position(target)
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
			local dx = tarx - x
			local dy = tary - y
			local hypot = math.sqrt(dx^2 + dy^2)
			if hypot > rn.entity.prefabs.zombie_ai.get_aggro_range(uuid) then
				-- if target is out of aggro range, clear target and move to its last known location.
				rn.current_scene():entity_write(uuid, "last_known_target_positionx", tarx)
				rn.current_scene():entity_write(uuid, "last_known_target_positiony", tary)
				rn.entity.prefabs.zombie_ai.set_target(uuid, nil)
			elseif hypot < 1.0 then
				-- if we're really close. attack!
				rn.spell.cast(uuid, "melee")
			else
				-- keep going.
				local can_move = rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
				if not can_move then
					-- cant move to target. just clear it and hope for the best?
					rn.entity.prefabs.zombie_ai.set_target(uuid, nil)
				end
			end
		else
			-- try to find target
			rn.entity.prefabs.zombie_ai.find_target(uuid, rn.entity.prefabs.zombie_ai.get_aggro_range(uuid))

			-- move to last known position for now.
			local tarx = rn.current_scene():entity_read(uuid, "last_known_target_positionx")
			local tary = rn.current_scene():entity_read(uuid, "last_known_target_positiony")
			if tarx ~= nil and tary ~= nil then
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)
				local dx = tarx - x
				local dy = tary - y
				local hypot = math.sqrt(dx^2 + dy^2)
				if hypot < 0.5 then
					print("dropping last known target. dx = " .. tostring(dx) .. ", dy = " .. tostring(dy))
					rn.current_scene():entity_write(uuid, "last_known_target_positionx", nil)
					rn.current_scene():entity_write(uuid, "last_known_target_positionx", nil)
					return
				end
				rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
			end
		end
	end,
	get_aggro_range = function(uuid)
		return rn.current_scene():entity_read(uuid, ".aggro_range") or 10.0
	end,
	set_aggro_range = function(uuid, aggro_range)
		rn.current_scene():entity_write(uuid, ".aggro_range", aggro_range)
	end
}