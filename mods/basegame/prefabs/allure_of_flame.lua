rn.mods.basegame.prefabs.allure_of_flame =
{
	description = "BIG FIREBALL GO BRRR WSSHHH KHHHRRRSCHHH",
	pre_instantiate = rn.mods.basegame.prefabs.firebolt.pre_instantiate,
	instantiate = rn.mods.basegame.prefabs.firebolt.instantiate,
	update = rn.mods.basegame.prefabs.firebolt.update,
	on_collision = rn.mods.basegame.prefabs.firebolt.on_collision,
	on_remove = function(uuid)
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		rn.mods.basegame.prefabs.firebolt.on_remove(uuid)
		local old_player = rn.level.data_read("allure_player")
		tz.assert(old_player ~= nil, "Big bad logic error")
		rn.level.data_write("player", old_player)
		rn.level.data_write("allure_player", nil)
		rn.entity.prefabs.sprite.set_position(old_player, x, y)
		rn.entity.prefabs.bipedal.play_animation(old_player, "Cast2H_Omni", false, 3.0)
		rn.spell.spells.firestorm.finish(old_player, x, y)
	end,
	set_damage = rn.mods.basegame.prefabs.firebolt.set_damage,
	get_damage = rn.mods.basegame.prefabs.firebolt.get_damage,
	set_target = rn.mods.basegame.prefabs.firebolt.set_target,
}