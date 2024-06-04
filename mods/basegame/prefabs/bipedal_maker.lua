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
	end
}