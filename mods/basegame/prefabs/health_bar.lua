rn.mods.basegame.prefabs.health_bar_impl =
{
	description = "Moving part of the health-bar on an entity. Attached to a `health_bar` parent.",
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local parent = sc:entity_read(uuid, "parent")
		if parent == nil then return end
		if not sc:contains_entity(parent) then
			-- parent is despawned. internal update is no longer being invoked - we are orphaned.
			-- we gotta just despawn ourselves.
			sc:remove_entity(uuid)
		end
	end,
	internal_update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local parent = sc:entity_read(uuid, "parent")
		if parent ~= nil and sc:contains_entity(parent) then
			local base_ox = sc:entity_read(uuid, "base_offsetx")
			local base_oy = sc:entity_read(uuid, "base_offsety")
			local offsetx = 0.0
			local offsety = 0.0

			local attachment = sc:entity_read(parent, "attachment")

			local max_hp = rn.entity.prefabs.combat_stats.get_max_hp(attachment)
			local hp = rn.entity.prefabs.combat_stats.get_hp(attachment)
			local hp_pct = hp / max_hp
			rn.entity.prefabs.sprite.set_colour(uuid, 1.0 - hp_pct, hp_pct * 0.5, 0.0)

			local health_bar_middle_scale = 0.8
			local padding = 1.1
			local scalex = 1.0 * health_bar_middle_scale * hp_pct * padding
			offsetx = ((1.0 - hp_pct) * -health_bar_middle_scale) * padding

			rn.entity.prefabs.sticky.set_offset(uuid, offsetx + base_ox, offsety + base_oy)
			rn.entity.prefabs.sticky.update(uuid, delta_seconds)
			rn.current_scene():entity_set_global_scale(uuid, health_bar_middle_scale, 0.2 * health_bar_middle_scale, scalex)
			rn.entity.prefabs.sprite.set_visible(uuid, true)
		end
	end
}

rn.mods.basegame.prefabs.absorb_bar_impl =
{
	description = "Absorb Bar on an entity",
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local parent = sc:entity_read(uuid, "parent")
		if parent == nil then return end
		if not sc:contains_entity(parent) then
			-- parent is despawned. internal update is no longer being invoked - we are orphaned.
			-- we gotta just despawn ourselves.
			sc:remove_entity(uuid)
		end
	end,
	internal_update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local parent = sc:entity_read(uuid, "parent")
		if parent ~= nil and sc:contains_entity(parent) then
			local base_ox = sc:entity_read(uuid, "base_offsetx")
			local base_oy = sc:entity_read(uuid, "base_offsety")
			local offsetx = 0.0
			local offsety = 0.0

			local attachment = sc:entity_read(parent, "attachment")

			local max_hp = rn.entity.prefabs.combat_stats.get_max_hp(attachment)
			local absorb = rn.entity.prefabs.combat_stats.get_absorb(attachment)
			local hp_pct = math.min(absorb / max_hp, 1.0)
			rn.entity.prefabs.sprite.set_colour(uuid, 1.0, 0.5, 0.0)

			local health_bar_middle_scale = 0.8
			local padding = 1.1
			local scalex = 1.0 * health_bar_middle_scale * hp_pct * padding
			offsetx = ((1.0 - hp_pct) * -health_bar_middle_scale) * padding

			rn.entity.prefabs.sticky.set_offset(uuid, offsetx + base_ox, offsety + base_oy)
			rn.entity.prefabs.sticky.update(uuid, delta_seconds)
			rn.current_scene():entity_set_global_scale(uuid, health_bar_middle_scale, 0.075 * health_bar_middle_scale, scalex)
			rn.entity.prefabs.sprite.set_visible(uuid, true)
		end
	end
}

rn.mods.basegame.prefabs.health_bar = 
{
	description = "Health-bar background display on an entity, showing their current health against their maximum health.",
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.current_scene():entity_set_local_scale(uuid, 1.0, 0.2, 1.0)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()

		local attachment = sc:entity_read(uuid, "attachment")
		if attachment ~= nil then
			local child = sc:entity_read(uuid, "child")
			tz.assert(child ~= nil and sc:contains_entity(child), "CHILD IS MCDEAD WTF")
			if not sc:contains_entity(attachment) then
				-- we were attached to an entity, but its now gone. we just delete ourselves.
				sc:remove_entity(uuid)
			else
				-- do our childs update too - otherwise its a frame behind and looks weird.
				rn.entity.prefabs.health_bar_impl.internal_update(child, delta_seconds)

				rn.entity.prefabs.sticky.update(uuid, delta_seconds)
				-- absorb text
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)

				local absorb_child = sc:entity_read(uuid, "absorb_child")
				if absorb_child ~= nil and sc:contains_entity(absorb_child) then
					rn.entity.prefabs.absorb_bar_impl.internal_update(absorb_child, delta_seconds)
				end

			end
		end
	end,
	on_remove = function(uuid)
		local sc = rn.current_scene()
		local attachment = sc:entity_read(uuid, "attachment")
		if attachment ~= nil and sc:contains_entity(attachment) then
			sc:entity_write(attachment, "health_bar_active", nil)
		end
	end,
	display = function(uuid, duration)
		local sc = rn.current_scene()
		local alive = rn.entity.prefabs.combat_stats.is_alive(uuid)
		local already_has = sc:entity_read(uuid, "health_bar_active")
		local display_disabled = sc:entity_read(uuid, "nohealthbar") ~= nil
		if not alive or display_disabled then return end
		if already_has ~= nil and sc:contains_entity(already_has) then
			-- already_has is the uuid of the existing health bar. extend its duration back.
			print("extending duration of " .. already_has)
			rn.util.entity_set_despawn_timer(already_has, duration)
			return
		end

		local me = sc:add_entity("health_bar")
		sc:entity_write(uuid, "health_bar_active", me)
		local base_offsetx = 0.0
		local base_offsety = -1.0
		local base_offsetz = 0.0
		sc:entity_write(me, "attachment", uuid)
		rn.entity.prefabs.sticky.stick_to(me, uuid, base_offsetx, base_offsety, base_offsetz)
		rn.util.entity_set_despawn_timer(me, duration)

		local inner = sc:add_entity("health_bar_impl")
		sc:entity_write(inner, "parent", me)
		sc:entity_write(me, "child", inner)
		-- teeny tiny Z offset to guarantee inner always displays over `uuid` (or it will be completely occluded!)
		rn.entity.prefabs.sticky.stick_to(inner, uuid, base_offsetx, base_offsety, base_offsetz + 0.01)
		sc:entity_write(inner, "base_offsetx", base_offsetx)
		sc:entity_write(inner, "base_offsety", base_offsety)

		local absorb_inner = sc:add_entity("absorb_bar_impl")
		sc:entity_write(absorb_inner, "parent", me)
		sc:entity_write(me, "absorb_child", absorb_inner)
		-- teeny tiny Z offset to guarantee absorb_inner always displays over `uuid` (or it will be completely occluded!)
		local absorb_offsety = -0.1
		rn.entity.prefabs.sticky.stick_to(absorb_inner, uuid, base_offsetx, base_offsety + absorb_offsety, base_offsetz + 0.015)
		sc:entity_write(absorb_inner, "base_offsetx", base_offsetx)
		sc:entity_write(absorb_inner, "base_offsety", base_offsety + absorb_offsety)
	end,
	never_display_on = function(uuid)
		rn.current_scene():entity_write(uuid, "nohealthbar", true)
	end
}