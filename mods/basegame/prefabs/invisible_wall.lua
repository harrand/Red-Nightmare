rn.mods.basegame.prefabs.invisible_wall =
{
	description = "1x1 Immovable object. Invisible, although has a debug-pink appearance on debug builds.",
	static_init = function()
		if tz.debug then
			rn.renderer():add_texture("invisible", "basegame/res/sprites/invisible_debug.png")
		else
			rn.renderer():add_texture("invisible", "basegame/res/sprites/invisible.png")
		end
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.obstacle.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "invisible")
	end,
	on_collision = rn.mods.basegame.prefabs.obstacle.on_collision
}