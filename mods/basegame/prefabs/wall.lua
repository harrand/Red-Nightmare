rn.mods.basegame.prefabs.wall =
{
	description = "1x1 Immovable object.",
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.obstacle.instantiate(uuid)
	end,
	on_collision = rn.mods.basegame.prefabs.obstacle.on_collision
}