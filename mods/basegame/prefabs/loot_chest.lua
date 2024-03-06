rn.mods.basegame.prefabs.loot_chest =
{
	description = "1x1 Immovable object. Invisible, although has a debug-pink appearance on debug builds.",
	static_init = function()
		rn.renderer():add_texture("sprite.chest", "basegame/res/sprites/chest/chest0.png")
		rn.renderer():add_texture("sprite.chest_normals", "basegame/res/sprites/chest/chest0_normals.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.obstacle.instantiate(uuid)
		rn.entity.prefabs.sprite.set_rotation(uuid, -math.pi / 2.0)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.chest")
		rn.entity.prefabs.sprite.set_normal_map(uuid, "sprite.chest_normals")
	end,
	on_remove = function(uuid)
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		local loot_count = rn.current_scene():entity_read(uuid, ".loot_count") or 0
		if loot_count == 0 then return end
		for i=1,loot_count,1 do
			local cur_loot = rn.current_scene():entity_read(uuid, ".loot" .. i)
			tz.assert(cur_loot ~= nil, "Loot entry in chest was somehow nil. Logic error.")
			rn.item.drop_at(x, y, cur_loot)
		end
	end,
	on_collision = function(me, other)
		if me == other then return true end
		local prefab = rn.current_scene():entity_read(other, ".prefab")
		local is_projectile = rn.current_scene():entity_read(other, ".is_projectile")
		if prefab == "melee_swing_area" or is_projectile then
			rn.current_scene():remove_entity(me)
			rn.current_scene():remove_entity(other)
		end
		return true
	end,
	add_loot = function(uuid, item_name)
		local sc = rn.current_scene()
		local loot_count = sc:entity_read(uuid, ".loot_count") or 0
		loot_count = loot_count + 1
		sc:entity_write(uuid, ".loot" .. loot_count, item_name)
		sc:entity_write(uuid, ".loot_count", loot_count)

		local loot_rarity = rn.item.items[item_name].rarity or "common"
		local rarity_id = rn.item.rarity[loot_rarity].impl_index
		local current_max_rarity = sc:entity_read(uuid, "max_rarity") or "common"
		local current_max_rarity_id = rn.item.rarity[current_max_rarity].impl_index

		-- the new loot is a higher rarity than any others so far - upgrade the chests rarity.
		if rarity_id > current_max_rarity_id then
			sc:entity_write(uuid, "max_rarity", loot_rarity)
			local col = rn.item.rarity[loot_rarity].colour
			rn.entity.prefabs.sprite.set_colour(uuid, col[1], col[2], col[3])
		end
	end,
	add_random_loot = function(uuid, rarity)
		local rarity_data = rn.item.rarity[rarity]
		tz.assert(rarity_data ~= nil, "Item rarity \"" .. tostring(rarity) .. "\" is seemingly invalid.")
		local rarity_item_count = #rarity_data.items
		local item_index = math.random(1, rarity_item_count)
		rn.entity.prefabs.loot_chest.add_loot(uuid, rarity_data.items[item_index])
	end
}