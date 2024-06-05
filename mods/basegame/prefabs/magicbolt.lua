rn.mods.basegame.prefabs.magic_ball_base =
{
	description = "Meta-Entity representing a bolt of unknown magic type.",
	frame_count = 3,
	static_init = function()
		for i=0,rn.entity.prefabs.magic_ball_base.frame_count,1 do
			rn.renderer():add_texture("sprite.magicball" .. i, "basegame/res/sprites/magic_ball/magic_ball" .. i .. ".png")
		end
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball0")
		rn.entity.prefabs.light_emitter.instantiate(uuid)
		rn.entity.prefabs.health_bar.never_display_on(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, ".boundary_scale", 0.5)
		sc:entity_write(uuid, ".is_projectile", true)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, 25)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local magic_type = sc:entity_read(uuid, "magic_type")
		if magic_type ~= "physical" then
			local t = sc:entity_read(uuid, "timer") or 0.0
			t = t + delta_seconds
			local frame_id = math.floor((t * 10.0) % rn.entity.prefabs.magic_ball_base.frame_count)
			rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball" .. frame_id)
			sc:entity_write(uuid, "timer", t)
		else
			rn.entity.prefabs.sprite.set_visible(uuid, false)
		end

		rn.entity.prefabs.mouse_controlled.update(uuid, delta_seconds)

		local tarx = sc:entity_read(uuid, "target_location_x")
		local tary = sc:entity_read(uuid, "target_location_y")
		if tarx ~= nil and tary ~= nil then
			rn.entity.prefabs.sprite.lookat(uuid, tarx, tary, math.pi / -2.0)
		end
		rn.entity.prefabs.timed_despawn.update(uuid, delta_seconds)

		if rn.entity.prefabs.light_emitter.exists(uuid) then
			if magic_type == "physical" then
				-- physical must remove its light instantly.
				rn.entity.prefabs.light_emitter.on_remove(uuid)
			else
				local scale = rn.entity.prefabs.sprite.get_scale(uuid)
				rn.entity.prefabs.light_emitter.set_power(uuid, 0.6 * scale * scale)
				local colour = rn.spell.schools[magic_type].colour
				rn.entity.prefabs.light_emitter.set_colour(uuid, colour[1], colour[2], colour[3])
				rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
			end
		end
	end,
	on_collision = function(uuid_a, uuid_b)
		-- todo: don't do this is the other thing is "friendly"
		-- despawn ourselves, then deal damage to the collidee with dmg equal to our max health.
		local dmg = rn.entity.prefabs.combat_stats.get_max_hp(uuid_a)
		local target_alive = rn.entity.prefabs.combat_stats.is_alive(uuid_b)
		local magic_type = rn.current_scene():entity_read(uuid_a, "magic_type")
		local owner_id = rn.current_scene():entity_read(uuid_a, "owner")
		local other_owner = rn.current_scene():entity_read(uuid_b, "owner")
		if magic_type == nil then
			magic_type = "physical"
		end
		-- if we touch an obstacle, die.
		if rn.entity.prefabs.obstacle.is_obstacle(uuid_b) then
			rn.current_scene():remove_entity(uuid_a)
			return false
		end
		if owner_id == uuid_b or not target_alive or (owner_id ~= nil and owner_id == other_owner) or rn.entity.prefabs.faction.is_ally(uuid_a, uuid_b) then
			-- collided with whomsoever casted me. don't do anything.
			return false
		end
		rn.entity.prefabs.combat_stats.dmg(uuid_b, dmg, magic_type, owner_id)
		local other_is_projectile = rn.current_scene():entity_read(uuid_b, ".is_projectile")
		if not other_is_projectile then
			if magic_type == "frost" then
				rn.buff.apply(uuid_b, "chilled")
			elseif magic_type == "fire" then
				if rn.buff.is_applied(uuid_b, "ignite") then
					-- ignites stack damage and reset duration.
					local damage = rn.current_scene():entity_read(uuid_b, "ignite_damage") or 1	
					rn.current_scene():entity_write(uuid_b, "ignite_damage", damage + dmg)
					local duration = rn.current_scene():entity_read(uuid_b, rn.buff.prefix("ignite", "duration"))
					rn.current_scene():entity_write(uuid_b, rn.buff.prefix("ignite", "duration"), duration + rn.buff.buffs.ignite.duration)
				else
					rn.buff.apply(uuid_b, "ignite")
					rn.current_scene():entity_write(uuid_b, "ignite_damage", dmg)
					rn.current_scene():entity_write(uuid_b, "ignite_cause", owner_id)
				end
			end
		end
		if not (other_is_projectile and magic_type == "shadow") then
			-- dont despawn shadow projectiles if it touches another.
			rn.current_scene():remove_entity(uuid_a)
		end
		return false
	end,
	on_remove = rn.mods.basegame.prefabs.light_emitter.on_remove,
	set_damage = function(uuid, dmg)
		-- damage dealt is equal to max hp.
		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, dmg)
	end,
	get_damage = function(uuid)
		return rn.entity.prefabs.combat_stats.get_base_max_hp(uuid)
	end,
	set_target = function(uuid, tarx, tary)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "target_location_x", tarx)
		sc:entity_write(uuid, "target_location_y", tary)
	end
}

-- for each magic school (that isnt physical), create a new prefab based on magic_ball_base.
-- e.g firebolt, frostbolt etc...
for schoolname, schooldata in pairs(rn.spell.schools) do
	-- is a magic school.
	rn.mods.basegame.prefabs[schoolname .. "bolt"] =
	{
		description = "Represents a magical bolt of " .. schoolname,
		pre_instantiate = rn.mods.basegame.prefabs.magic_ball_base.pre_instantiate,
		instantiate = function(uuid)
			rn.mods.basegame.prefabs.magic_ball_base.instantiate(uuid)
			rn.current_scene():entity_write(uuid, "magic_type", schoolname)

			-- todo: saner magic colours
			if schoolname ~= "physical" then
				local school = rn.spell.schools[schoolname]
				rn.entity.prefabs.sprite.set_colour(uuid, school.colour[1], school.colour[2], school.colour[3])
			end
		end,
		update = rn.mods.basegame.prefabs.magic_ball_base.update,
		on_collision = function(me, other)
			-- special behaviour:
			-- if the magicbolt hits a dropped itemset containing an elemental circlet - it ignites it.
			local other_prefab = rn.current_scene():entity_read(other, ".prefab")
			if other_prefab == "loot_drop" and schoolname ~= "physical" then
				local circlet = "elemental_circlet_base"
				local circlet_slot = rn.item.items[circlet].slot
				if rn.item.get_equipped(other, circlet_slot) == circlet then
					-- the loot drop needs to be animated into an elemental of our type.
					local elemental = rn.current_scene():add_entity(schoolname .. "_elemental")
					rn.entity.prefabs.sprite.set_position(elemental, rn.entity.prefabs.sprite.get_position(other))
					rn.item.move_equipment(other, elemental)
					rn.current_scene():remove_entity(other)
					rn.current_scene():remove_entity(me)
				end
			end

			return rn.mods.basegame.prefabs.magic_ball_base.on_collision(me, other)
		end,
		on_remove = rn.mods.basegame.prefabs.magic_ball_base.on_remove
	}
end