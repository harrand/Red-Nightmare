rn.mods.basegame.prefabs.boss_pyr =
{
	description = "The god of fire, Pyr",
	pre_instantiate = rn.mods.basegame.prefabs.bipedal.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.maker")
		rn.entity.prefabs.bipedal.set_scale(uuid, 4.0, 4.0, 4.0)
		rn.entity.prefabs.bipedal.set_drop_items_on_death(uuid, false)

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 5000)
		rn.entity.prefabs.combat_stats.set_base_physical_power(uuid, 2.0)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, 0.0)
		rn.entity.prefabs.combat_stats.set_base_haste(uuid, -0.5)
		rn.entity.prefabs.combat_stats.set_base_fire_power(uuid, 5.0)

		rn.entity.prefabs.light_emitter.instantiate(uuid)

		rn.entity.prefabs.ranged_ai.instantiate(uuid)
		rn.entity.prefabs.ranged_ai.set_aggro_range(uuid, 25)
		rn.current_scene():entity_write(uuid, ".elemental_magic_type", "fire")
		rn.entity.prefabs.base_ai.add_ability(uuid, "firestorm", rn.ai.ability.filler_damage)
	end,
	update = rn.mods.basegame.prefabs.elemental_base.update,
	on_struck = rn.mods.basegame.prefabs.ranged_ai.on_struck,
	on_move = rn.mods.basegame.prefabs.bipedal.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.bipedal.on_stop_moving,
	on_collision = rn.mods.basegame.prefabs.bipedal.on_collision,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin,
	on_cast_success = rn.mods.basegame.prefabs.ranged_ai.on_cast_success,
	on_death = rn.mods.basegame.prefabs.elemental_base.on_death,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
}