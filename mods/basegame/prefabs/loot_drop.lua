rn.mods.basegame.prefabs.loot_drop =
{
	description = "Meta Entity representing a dropped item (or equipment set)",
	pre_instantiate = rn.mods.basegame.prefabs.bipedal.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_scale(uuid, 0.75, 0.75, 0.75)
		rn.entity.prefabs.bipedal.set_visible(uuid, false)
		rn.entity.prefabs.light_emitter.instantiate(uuid)
	end,
	on_remove = function(uuid)
		rn.entity.prefabs.light_emitter.on_remove(uuid)
	end,
	update = function(uuid, delta_seconds)
		-- dont do this - it will run animiations
		-- rn.entity.prefabs.bipedal.update(uuid, delta_seconds)
		-- we always stick with the default t-pose.

		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "timer") or 0.0
		t = t + delta_seconds * 2
		sc:entity_write(uuid, "timer", t)

		local xrot = 0.0
		local yrot = t
		local zrot = 0.0
		-- if we are displaying a weapon - rotate slightly differently or the weapon could clip through the near/far planes, or worse, the terrain
		if rn.item.get_equipped(uuid, rn.item.slot.right_hand) ~= nil or rn.item.get_equipped(uuid, rn.item.slot.left_hand) ~= nil then
			xrot = -math.pi * 0.5
			yrot = 0.0
			zrot = t * 0.5
			rn.entity.prefabs.bipedal.set_scale(uuid, 0.5, 0.5, 0.5)
		end
		rn.entity.prefabs.bipedal.set_rotation(uuid, xrot, yrot, zrot)
		rn.entity.prefabs.bipedal.set_position(uuid, 0.0, 0.2 * math.sin(t), 0.0)

		if rn.entity.prefabs.light_emitter.exists(uuid) then
			rn.entity.prefabs.light_emitter.set_power(uuid, 1.25)
			rn.entity.prefabs.light_emitter.set_colour(uuid, 1.0, 1.0, 1.0)
			rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
		end
	end,
	get_position = rn.mods.basegame.prefabs.bipedal.get_position,
	set_position = rn.mods.basegame.prefabs.bipedal.set_position,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
	on_collision = function(me, other)
		local should_equip_me = rn.entity.prefabs.bipedal.get_can_equip(other) and rn.entity.prefabs.combat_stats.is_alive(other)
		if not should_equip_me then
			return false
		end

		rn.item.move_equipment(me, other)
		rn.current_scene():remove_entity(me)
		return false
	end,
}