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

		local blacklist_timer = sc:entity_read(uuid, "pickup_blacklist_timer") or 0.0
		-- after 2.5 seconds have passed, the blacklist is automatically cleared
		-- note 5.0 = 2.5 * 2 coz timer moves at double speed.
		if blacklist_timer ~= nil and t > blacklist_timer + 5.0 then
			rn.entity.prefabs.loot_drop.clear_blacklisted(uuid)
		end

		local xrot = 0.0
		local yrot = t
		local zrot = 0.0
		-- if we aren't displaying armour - rotate slightly differently or the weapon could clip through the near/far planes, or worse, the terrain
		local contains_armour = false
		for i=1,rn.item.slot.left_hand-1,1 do
			local eq = rn.item.get_equipped(uuid, i)
			if eq ~= nil then
				contains_armour = true
			end
		end
		if not contains_armour then
			xrot = -math.pi * 0.5
			yrot = 0.0
			zrot = t * 0.5
			rn.entity.prefabs.bipedal.set_scale(uuid, 0.5, 0.5, 0.5)
		end
		rn.entity.prefabs.bipedal.set_rotation(uuid, xrot, yrot, zrot)
		local x, y, z = rn.entity.prefabs.bipedal.get_position(uuid)
		rn.entity.prefabs.bipedal.set_position(uuid, x, y + 0.01 * math.sin(t), z)
		local max_rarity = rn.item.get_highest_equipped_rarity(uuid)
		local rarity_colour = rn.item.rarity[max_rarity].colour

		if rn.entity.prefabs.light_emitter.exists(uuid) then
			rn.entity.prefabs.light_emitter.set_power(uuid, 0.6)
			rn.entity.prefabs.light_emitter.set_colour(uuid, rarity_colour[1], rarity_colour[2], rarity_colour[3])
			rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
		end
	end,
	get_position = rn.mods.basegame.prefabs.bipedal.get_position,
	set_position = rn.mods.basegame.prefabs.bipedal.set_position,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
	on_collision = function(me, other)
		local should_equip_me = rn.entity.prefabs.bipedal.get_can_equip(other) and rn.entity.prefabs.combat_stats.is_alive(other) and rn.current_scene():entity_read(me, "pickup_blacklist") ~= other
		if not should_equip_me then
			return false
		end

		rn.item.move_equipment(me, other)
		rn.current_scene():remove_entity(me)
		return false
	end,
	clear_blacklisted = function(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "pickup_blacklist_timer", nil)
		sc:entity_write(uuid, "pickup_blacklist", nil)
	end,
	set_blacklisted = function(uuid, other_uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "pickup_blacklist_timer", sc:entity_read(uuid, "timer") or 0.0)
		sc:entity_write(uuid, "pickup_blacklist", other_uuid)
	end
}