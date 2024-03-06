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
		rn.item.drop_at(x, y, "iron_chainmail")
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
}