rn.mods.basegame.prefabs.bipedal_maker =
{
	description = "Entity is a 3D bipedal maker.",
	static_init = function()
		rn.renderer():add_texture("skin.maker", "basegame/res/skins/maker.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.bipedal.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_scale(uuid, 0.75, 1.05, 0.95)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.maker")
	end,
	update = rn.mods.basegame.prefabs.bipedal.update,
	on_collision = rn.mods.basegame.prefabs.bipedal.on_collision,
	on_move = rn.mods.basegame.prefabs.bipedal.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.bipedal.on_stop_moving,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.bipedal.on_death,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
	play_animation = rn.mods.basegame.prefabs.bipedal.play_animation,
}