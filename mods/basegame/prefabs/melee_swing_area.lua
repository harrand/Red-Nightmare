rn.mods.basegame.prefabs.melee_swing_area =
{
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_visible(uuid, false)
	end,
	update = function(uuid, delta_seconds)
		rn.entity.prefabs.timed_despawn.update(uuid, delta_seconds)
	end,
	on_collision = function(me, other)
		local sc = rn.current_scene()
		local other_alive = rn.entity.prefabs.combat_stats.is_alive(other)
		local ally = rn.entity.prefabs.faction.is_ally(me, other)
		-- dont hit dead enemies or allies
		if not other_alive or ally then return false end

		local caster = rn.entity.prefabs.melee_swing_area.get_caster(me)
		-- dont hit if the caster is gone.
		if caster == nil or not sc:contains_entity(caster) then return false end
		-- dont hit your own caster.
		if caster == other then return false end

		local other_is_projectile = sc:entity_read(other, ".is_projectile") == true
		local other_owner = sc:entity_read(other, "owner")
		if other_is_projectile then
			if other_owner ~= caster then
				-- cool thing! we're gonna send the projectile away from the caster!
				-- if owner is the player - send it towards the mouse
				-- otherwise if owner is an ai with a target - send it towards the target
				-- else simply do a vector from base position
				local dx, dy = rn.util.entity_direction_to_target(caster, other)
				local casterx, castery = rn.entity.prefabs.sprite.get_position(caster)
				rn.entity.prefabs.magic_ball_base.set_target(other, casterx + (dx * 999), castery + (dy * 999))
				-- also set the owner to the caster now, so it can actually hit its enemies.
				sc:entity_write(other, "owner", caster)
				rn.entity.prefabs.faction.copy_faction(caster, other)
			end
		else
			-- deal some deeps
			rn.entity.prefabs.combat_stats.dmg(other, rn.entity.prefabs.melee_swing_area.get_base_damage(me), "physical", caster)
			local buff = rn.entity.prefabs.melee_swing_area.get_buff_on_hit(me)
			if buff ~= nil then
				rn.buff.apply(other, buff)
			end
			local stun = rn.entity.prefabs.melee_swing_area.get_stun_on_hit(me)
			if stun ~= nil then
				rn.entity.stun(other, stun)
			end
			sc:remove_entity(me)
		end

		return false
	end,
	get_base_damage = function(uuid)
		return rn.current_scene():entity_read(uuid, "melee_strength") or 1.0
	end,
	set_base_damage = function(uuid, dmg)
		rn.current_scene():entity_write(uuid, "melee_strength", dmg)
	end,
	set_caster = function(uuid, caster)
		rn.current_scene():entity_write(uuid, "melee_caster", caster)
	end,
	get_caster = function(uuid)
		return rn.current_scene():entity_read(uuid, "melee_caster")
	end,
	add_buff_on_hit = function(uuid, buffname)
		rn.current_scene():entity_write(uuid, "buff_on_hit", buffname)
	end,
	get_buff_on_hit = function(uuid)
		return rn.current_scene():entity_read(uuid, "buff_on_hit")
	end,
	set_stun_on_hit = function(uuid, stun_duration)
		rn.current_scene():entity_write(uuid, "stun_on_hit", stun_duration)
	end,
	get_stun_on_hit = function(uuid, stun_duration)
		return rn.current_scene():entity_read(uuid, "stun_on_hit")
	end
}