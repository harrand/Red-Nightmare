rn.mods.basegame.prefabs.weapon_model_morning_star =
{
	static_init = function()
		rn.renderer():add_model("weapon.morning_star", "basegame/res/models/morning_star.glb")
	end,
	pre_instantiate = function(uuid)
		return "weapon.morning_star"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_set_subobject_pixelated(uuid, 2, true)
		sc:entity_set_subobject_visible(uuid, 2, true)
	end,
	update = rn.mods.basegame.prefabs.sticky.update
}