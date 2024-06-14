rn.mods.basegame.prefabs.consecration_effect =
{
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	static_init = function()
		rn.renderer():add_texture("sprite.consecration", "basegame/res/sprites/consecrate.png")
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.consecration")
		rn.entity.prefabs.sprite.set_emissive_map(uuid, "sprite.consecration")
		rn.entity.prefabs.light_emitter.instantiate(uuid)
	end,
	update = function(uuid, delta_seconds)
		local timer = rn.current_scene():entity_read(uuid, "tick_timer") or 0.0		
		timer = timer + delta_seconds
		if timer >= 1.0 then
			rn.current_scene():entity_write(uuid, "tick_ready", true)
			timer = 0.0
		end
		rn.current_scene():entity_write(uuid, "tick_timer", timer)

		local magic_type = rn.current_scene():entity_read(uuid, "magic_type")
		if magic_type ~= nil then
			local colour = rn.spell.schools[magic_type].colour
			rn.entity.prefabs.sprite.set_colour(uuid, colour[1], colour[2], colour[3])
			rn.entity.prefabs.sprite.set_emissive_tint(uuid, colour[1], colour[2], colour[3])
			rn.entity.prefabs.light_emitter.set_power(uuid, rn.entity.prefabs.sprite.get_scale(uuid) * 0.5)
			rn.entity.prefabs.light_emitter.set_colour(uuid, colour[1], colour[2], colour[3])
		end

		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
	end,
	on_remove = rn.mods.basegame.prefabs.light_emitter.on_remove,
	on_collision = function(me, other)
		local sc = rn.current_scene()
		local other_alive = rn.entity.prefabs.combat_stats.is_alive(other)
		local ally = rn.entity.prefabs.faction.is_ally(me, other)
		local magic_type = sc:entity_read(me, "magic_type")

		local tick_ready = sc:entity_read(me, "tick_ready")
		-- dont hit anything dead
		if not tick_ready or not other_alive then return false end

		local caster = rn.entity.prefabs.consecration_effect.get_caster(me)
		-- dont hit if the caster is gone.
		if caster == nil or not sc:contains_entity(caster) then return false end

		local other_is_projectile = sc:entity_read(other, ".is_projectile") == true
		if not other_is_projectile then
			-- deal some deeps
			if ally then
				rn.entity.prefabs.combat_stats.heal(other, rn.entity.prefabs.consecration_effect.get_base_damage(me) * 0.5, magic_type, caster)
			else
				rn.entity.prefabs.combat_stats.dmg(other, rn.entity.prefabs.consecration_effect.get_base_damage(me), magic_type, caster)
			end

			local effect = sc:add_entity("cast_buildup")
			sc:entity_write(effect, "magic_type", magic_type)
			local yoffset = 2.0
			local effect_duration = 0.75
			rn.entity.prefabs.sticky.stick_to(effect, other, 0.0, yoffset, 0.0)
			rn.util.entity_set_despawn_timer(effect, effect_duration)

			local buff = rn.entity.prefabs.consecration_effect.get_buff_on_hit(me)
			if buff ~= nil then
				rn.buff.apply(other, buff)
			end
			local stun = rn.entity.prefabs.consecration_effect.get_stun_on_hit(me)
			if stun ~= nil then
				rn.entity.stun(other, stun)
			end
			-- we've ticked. next frame wont tick until tick_ready is set again
			sc:entity_write(me, "tick_ready", false)
		end

		return false
	end,
	get_base_damage = function(uuid)
		return rn.current_scene():entity_read(uuid, "consecration_strength") or 1.0
	end,
	set_base_damage = function(uuid, dmg)
		rn.current_scene():entity_write(uuid, "consecration_strength", dmg)
	end,
	set_caster = function(uuid, caster)
		rn.current_scene():entity_write(uuid, "consecration_caster", caster)
	end,
	get_caster = function(uuid)
		return rn.current_scene():entity_read(uuid, "consecration_caster")
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