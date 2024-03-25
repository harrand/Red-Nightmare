rn.mods.basegame.prefabs.skeleton =
{
	description = "A Skeleton.",
	static_init = function()
		rn.renderer():add_texture("skin.skeleton", "basegame/res/skins/skeleton.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.melee_monster.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.melee_monster.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.skeleton")
		rn.entity.prefabs.bipedal.set_drop_items_on_death(uuid, false)

		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed * 0.7)
	end,
	update = rn.mods.basegame.prefabs.melee_monster.update,
	on_struck = rn.mods.basegame.prefabs.melee_monster.on_struck,
	on_move = rn.mods.basegame.prefabs.melee_monster.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.melee_monster.on_stop_moving,
	on_collision = rn.mods.basegame.prefabs.melee_monster.on_collision,
	on_cast_begin = rn.mods.basegame.prefabs.melee_monster.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.melee_monster.on_death,
	on_equip = rn.mods.basegame.prefabs.melee_monster.on_equip,
	on_unequip = rn.mods.basegame.prefabs.melee_monster.on_unequip,
}