rn.mods.basegame.prefabs.player_melistra =
{
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
		rn.entity.prefabs.sprite.set_colour(uuid, 0.6, 0.3, 0.2)
		rn.entity.prefabs.sprite.set_scale(uuid, 0.2)
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 1, "lesser_firebolt")

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 100)
		rn.entity.prefabs.combat_stats.set_base_fire_power(uuid, 10)
		rn.entity.prefabs.combat_stats.set_base_fire_resist(uuid, 0.1)
	end,
	update = function(uuid, delta_seconds)
		rn.entity.prefabs.keyboard_controlled.update(uuid, delta_seconds)
	end,
	on_collision = function(me, other)
		return true
	end
}