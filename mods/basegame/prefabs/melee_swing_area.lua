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
		-- dont hit dead enemies
		if not other_alive then return false end

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
				local casterx, castery = rn.entity.prefabs.sprite.get_position(me)
				local projx, projy = rn.entity.prefabs.sprite.get_position(other)
				local vectorx = (casterx - projx) * 999
				local vectory = (castery - projy) * 999
				rn.entity.prefabs.magic_ball_base.set_target(other, projx - vectorx, projy - vectory)
				-- also set the owner to the caster now, so it can actually hit its enemies.
				sc:entity_write(other, "owner", caster)
			end
		else
			-- deal some deeps
			rn.entity.prefabs.combat_stats.dmg(other, 1, "physical", caster)
			sc:remove_entity(me)
		end

		return false
	end,
	set_caster = function(uuid, caster)
		rn.current_scene():entity_write(uuid, "melee_caster", caster)
	end,
	get_caster = function(uuid)
		return rn.current_scene():entity_read(uuid, "melee_caster")
	end
}