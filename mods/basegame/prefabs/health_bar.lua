rn.mods.basegame.prefabs.health_bar_impl =
{
	description = "Moving part of the health-bar on an entity. Attached to a `health_bar` parent.",
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_colour(uuid, 0.0, 1.0, 0.0)
		local health_bar_middle_scale = 0.8
		rn.current_scene():entity_set_local_scale(uuid, 1.0 * health_bar_middle_scale, 0.2 * health_bar_middle_scale, 1.0)
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
		local offsetx = 0.0
		local offsety = 0.0
		local parent = sc:entity_read(uuid, "parent")
		if parent ~= nil and sc:contains_entity(parent) then
			-- z offset of -0.1 so it always displays infront of the background
			rn.entity.prefabs.sticky.update(uuid, delta_seconds)
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
				rn.entity.prefabs.timed_despawn.update(uuid, delta_seconds)
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
		local already_has = sc:entity_read(uuid, "health_bar_active") ~= nil
		if not alive or already_has then return end

		sc:entity_write(uuid, "health_bar_active", true)

		local me = sc:add_entity("health_bar")
		sc:entity_write(me, "attachment", uuid)
		rn.entity.prefabs.sticky.stick_to(me, uuid, 0.0, -1.0, 0.0)
		rn.entity.prefabs.timed_despawn.set_duration(me, duration)

		local inner = sc:add_entity("health_bar_impl")
		sc:entity_write(inner, "parent", me)
		sc:entity_write(me, "child", inner)
		rn.entity.prefabs.sticky.stick_to(inner, uuid, 0.0, -1.0, 0.01)
	end
}