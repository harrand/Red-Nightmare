rn.mods.basegame.prefabs.weapon_small_shield =
{
	static_init = function()
		rn.renderer():add_model("weapon.small_shield", "basegame/res/models/small_shield.glb")
	end,
	pre_instantiate = function(uuid)
		return "weapon.small_shield"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_set_subobject_pixelated(uuid, 2, true)
		sc:entity_set_subobject_visible(uuid, 2, true)
	end,
	update = rn.mods.basegame.prefabs.sticky.update
}