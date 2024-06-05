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
		if owner ~= nil and rn.current_scene():contains_entity(owner) and (other ~= owner) then
			local owner_prefab = nil
			local owner_prefab_str = rn.current_scene():entity_read(owner, ".ai")
			if owner_prefab_str ~= nil then
				owner_prefab = rn.entity.prefabs[owner_prefab_str]
			end
			if rn.entity.prefabs.faction.is_enemy(me, other) and owner_prefab ~= nil then
				if owner_prefab.on_target_field_collide ~= nil then
					owner_prefab.on_target_field_collide(owner, me, other)
					return false
				end
			end
		end
		return false
	end,
}

rn.mods.basegame.prefabs.base_ai =
{
	instantiate = function(uuid)
		rn.current_scene():entity_write(uuid, ".ai", "base_ai")
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
		if target_field ~= nil and sc:contains_entity(target_field) then
			rn.entity.prefabs.sprite.set_scale(target_field, target_range)
		else
			target_field = sc:add_entity("basic_target_field")
			rn.entity.prefabs.faction.copy_faction(uuid, target_field)
			--rn.entity.prefabs.sprite.set_scale(target_field, target_range)
			sc:entity_write(target_field, "owner", uuid)
			rn.entity.prefabs.sticky.stick_to(target_field, uuid)
			sc:entity_write(uuid, "target_field", target_field)
		end
	end,
	on_target_field_collide = function(uuid, field_uuid, collided_uuid)
		local existing_target = rn.entity.prefabs.base_ai.get_target(uuid)
		if existing_target == nil or not rn.current_scene():contains_entity(existing_target) then
			-- need a new target.
			if rn.entity.prefabs.combat_stats.is_alive(collided_uuid) then
				rn.entity.prefabs.base_ai.set_target(uuid, collided_uuid)
				rn.current_scene():remove_entity(field_uuid)
				rn.current_scene():entity_write(uuid, "target_field", nil)
			end
		end
	end,
	update = function(uuid, delta_seconds)
		local target = rn.entity.prefabs.base_ai.get_target(uuid)
		if target ~= nil
		and rn.current_scene():contains_entity(target) then
			if not rn.entity.prefabs.combat_stats.is_alive(target) then
				-- target died. clear target and abort.
				rn.entity.prefabs.base_ai.set_target(uuid, nil)
				return
			else
				local tarx, tary = rn.entity.prefabs.sprite.get_position(target)
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)
				local dx = tarx - x
				local dy = tary - y
				local hypot = math.sqrt(dx^2 + dy^2)
				if hypot > rn.entity.prefabs.base_ai.get_aggro_range(uuid) then
					-- if target is out of aggro range, clear target and move to its last known location.
					rn.current_scene():entity_write(uuid, "last_known_target_positionx", tarx)
					rn.current_scene():entity_write(uuid, "last_known_target_positiony", tary)
					rn.entity.prefabs.base_ai.set_target(uuid, nil)
				end
			end
		else
			-- try to find target
			rn.entity.prefabs.base_ai.find_target(uuid, rn.entity.prefabs.base_ai.get_aggro_range(uuid))
		end
	end,
	on_struck = function(me, attacker, dmg, magic_type)
		local sc = rn.current_scene()
		if attacker ~= nil and sc:contains_entity(attacker) then
			local parent = sc:entity_read(attacker, "owner")
			if parent ~= nil and sc:contains_entity(parent) then attacker = parent end
			rn.entity.prefabs.base_ai.set_target(me, attacker)
			print("i am angered by " .. sc:entity_get_name(attacker))
		end
		return dmg
	end,
	get_aggro_range = function(uuid)
		return rn.current_scene():entity_read(uuid, ".aggro_range") or 10.0
	end,
	set_aggro_range = function(uuid, aggro_range)
		rn.current_scene():entity_write(uuid, ".aggro_range", aggro_range)
	end
}

rn.mods.basegame.prefabs.melee_ai =
{
	instantiate = rn.mods.basegame.prefabs.base_ai.instantiate,
	get_target = rn.mods.basegame.prefabs.base_ai.get_target,
	set_target = rn.mods.basegame.prefabs.base_ai.set_target,
	find_target = rn.mods.basegame.prefabs.base_ai.find_target,
	on_target_field_collide = rn.mods.basegame.prefabs.base_ai.on_target_field_collide,
	on_collision = function(me, other)
		local ret = rn.entity.prefabs.bipedal.on_collision(me, other)
		if not rn.entity.prefabs.combat_stats.is_alive(me) then return ret end
		local target = rn.entity.prefabs.base_ai.get_target(me)
		if target == other then
			local x, y = rn.entity.prefabs.sprite.get_position(me)
			local tarx, tary = rn.entity.prefabs.sprite.get_position(other)
			rn.entity.prefabs.spell_slots.cast_spell_at_slot(me, "green")
			rn.entity.prefabs.bipedal.face_direction(me, x - tarx, y - tary)
		end
		return ret
	end,
	update = function(uuid, delta_seconds)
		rn.entity.prefabs.base_ai.update(uuid, delta_seconds)
		-- if enemy is casting, let them cast.
		if rn.spell.is_casting(uuid) then return end

		-- if we're low hp, try casting yellow
		local max_hp = rn.entity.prefabs.combat_stats.get_max_hp(uuid)
		local hp = rn.entity.prefabs.combat_stats.get_hp(uuid)
		local hp_pct = hp / max_hp
		if hp_pct < 0.4 then
			local yellow_cd = rn.entity.prefabs.spell_slots.get_spell_cooldown(uuid, "yellow") or 0.0
			local yellow_spell = rn.entity.prefabs.spell_slots.get_spell(uuid, "yellow")
			if yellow_spell ~= nil and yellow_cd <= 0.0 then
				-- cast its yellow
				rn.entity.prefabs.spell_slots.cast_spell_at_slot(uuid, "yellow")
				return
			end
		end

		local target = rn.entity.prefabs.base_ai.get_target(uuid)

		if target ~= nil and rn.current_scene():contains_entity(target) then
			if not rn.spell.is_casting(uuid) then
				-- keep going.
				local tarx, tary = rn.entity.prefabs.sprite.get_position(target)
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)
				local dx = tarx - x
				local dy = tary - y
				local can_move = rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
				if not can_move then
					-- cant move to target. just clear it and hope for the best?
					rn.entity.prefabs.base_ai.set_target(uuid, nil)
				end
			end
		else
			-- move to last known position for now.
			local tarx = rn.current_scene():entity_read(uuid, "last_known_target_positionx")
			local tary = rn.current_scene():entity_read(uuid, "last_known_target_positiony")
			if tarx ~= nil and tary ~= nil then
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)
				local dx = tarx - x
				local dy = tary - y
				local hypot = math.sqrt(dx^2 + dy^2)
				if hypot < 0.5 then
					rn.current_scene():entity_write(uuid, "last_known_target_positionx", nil)
					rn.current_scene():entity_write(uuid, "last_known_target_positionx", nil)
					return
				end
				rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
			end
		end
	end,
	on_struck = rn.mods.basegame.prefabs.base_ai.on_struck,
	get_aggro_range = rn.mods.basegame.prefabs.base_ai.get_aggro_range,
	set_aggro_range = rn.mods.basegame.prefabs.base_ai.set_aggro_range,
}

rn.mods.basegame.prefabs.ranged_ai =
{
	instantiate = rn.mods.basegame.prefabs.base_ai.instantiate,
	get_target = rn.mods.basegame.prefabs.base_ai.get_target,
	set_target = rn.mods.basegame.prefabs.base_ai.set_target,
	find_target = rn.mods.basegame.prefabs.base_ai.find_target,
	on_cast_success = function(uuid, spellname, castx, casty)
		local random_multiplier = rn.current_scene():entity_write(uuid, "flee_multiplier", math.random(-100, 100) * 0.005)
	end,
	on_target_field_collide = rn.mods.basegame.prefabs.base_ai.on_target_field_collide,
	update = function(uuid, delta_seconds)
		rn.entity.prefabs.base_ai.update(uuid, delta_seconds)
		-- if enemy is casting, let them cast.
		local target = rn.entity.prefabs.base_ai.get_target(uuid)
		if target == nil or not rn.current_scene():contains_entity(target) then
			-- if target ever drops, cancel our current fleeing if we are.
			-- otherwise a new challenger approaches and the dude instantly goes french???
			rn.entity.prefabs.ranged_ai.set_fleeing(uuid, nil)
			return
		end
		local tarx, tary = rn.entity.prefabs.sprite.get_position(target)
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		local dx = tarx - x
		local dy = tary - y
		local hypot = math.sqrt(dx^2 + dy^2)
		local aggro_range = rn.entity.prefabs.base_ai.get_aggro_range(uuid)

		if rn.entity.prefabs.ranged_ai.is_fleeing(uuid) then
			-- we're currently fleeing.
			-- -dx, -dy would be exactly away from the enemy.
			-- but we want to introduce a lil bit of randomness.
			-- however it cant change every frame, so we simply multiply dx by our uuid.
			local runaway_timer = rn.current_scene():entity_read(uuid, "flee_duration") or 0.0
			-- if we've ran halfway to our aggro range, then we've gone far enough and can stop running now
			if hypot >= (aggro_range * 0.5) then
				runaway_timer = 0
			end
			local flee_multiplier = rn.current_scene():entity_read(uuid, "flee_multiplier") or 0.0
			rn.entity.on_move(uuid, -dx + flee_multiplier, -dy, 0.0, delta_seconds)
			rn.entity.prefabs.ranged_ai.set_fleeing(uuid, runaway_timer - delta_seconds)
		elseif not rn.spell.is_casting(uuid) then
			rn.entity.prefabs.bipedal.face_direction(uuid, -dx, -dy)
			if hypot >= (aggro_range * 0.8) then
				-- ranged will move closer if its target is very far.
				rn.entity.on_move(uuid, dx, dy, 0.0, delta_seconds)
			elseif hypot <= (aggro_range * 0.2) then
				-- ranged will move away if its target is too close (upto a maximum amount of time running)
				local max_hp = rn.entity.prefabs.combat_stats.get_max_hp(uuid)
				local hp = rn.entity.prefabs.combat_stats.get_hp(uuid)
				local hp_pct = hp / max_hp
				if hp_pct < 0.4 then
					local yellow_cd = rn.entity.prefabs.spell_slots.get_spell_cooldown(uuid, "yellow") or 0.0
					local yellow_spell = rn.entity.prefabs.spell_slots.get_spell(uuid, "yellow")
					if yellow_spell ~= nil and yellow_cd <= 0.0 then
						-- cast its yellow
						rn.entity.prefabs.spell_slots.cast_spell_at_slot(uuid, "yellow")
						return
					elseif rn.entity.prefabs.ranged_ai.can_flee(uuid) then
						-- uhh run away?
						rn.entity.prefabs.ranged_ai.set_fleeing(uuid, 5.0)
					end
					-- fallback to normal behavior below.
				end
			end
			-- if we get to this point, cast spells.
			rn.entity.prefabs.spell_slots.cast_spell_at_slot(uuid, "green")
		end
	end,
	on_struck = rn.mods.basegame.prefabs.base_ai.on_struck,
	get_aggro_range = rn.mods.basegame.prefabs.base_ai.get_aggro_range,
	set_aggro_range = rn.mods.basegame.prefabs.base_ai.set_aggro_range,
	set_fleeing = function(uuid, flee_duration)
		rn.current_scene():entity_write(uuid, "flee_duration", flee_duration)
	end,
	is_fleeing = function(uuid)
		return (rn.current_scene():entity_read(uuid, "flee_duration") or 0.0) > 0.0
	end,
	can_flee = function(uuid)
		-- can flee by default. do set_can_flee(uuid, false) to disable fleeing.
		local flee = rn.current_scene():entity_read(uuid, "can_flee")
		if flee == false then return false end
		return true
	end,
	set_can_flee = function(uuid, can_flee)
		rn.current_scene():entity_write(uuid, "can_flee", can_flee)
	end
}