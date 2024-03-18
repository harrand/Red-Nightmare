rn.mods.basegame.prefabs.zombie =
{
	description = "A Zombie.",
	static_init = function()
		rn.renderer():add_texture("skin.zombie", "basegame/res/skins/zombie_melistra.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.melee_monster.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.melee_monster.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.zombie")
		rn.entity.prefabs.bipedal.set_run_animation(uuid, "ZombieRun")
		rn.entity.prefabs.bipedal.set_idle_animation(uuid, "ZombieIdle")
		rn.entity.prefabs.bipedal.set_death_animation(uuid, "ZombieDeath")

		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed * 0.7)

		rn.entity.prefabs.melee_ai.set_melee_ability(uuid, "zombie_swipe")
	end,
	update = rn.mods.basegame.prefabs.melee_monster.update,
	on_move = rn.mods.basegame.prefabs.melee_monster.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.melee_monster.on_stop_moving,
	on_collision = function(me, other)
		local target = rn.entity.prefabs.base_ai.get_target(me)
		if rn.entity.prefabs.combat_stats.is_alive(me) and target == other and rn.entity.prefabs.faction.is_enemy(me, other) and not rn.spell.is_casting(me) then
			local attack_power = rn.entity.prefabs.combat_stats.get_physical_power(me)
			local target_hp = rn.entity.prefabs.combat_stats.get_hp(target)
			-- devour if enemy is below 25% health or our attack power exceeds their current health
			local target_max_hp = rn.entity.prefabs.combat_stats.get_max_hp(target)
			if target_hp > 0 and ((attack_power >= target_hp - attack_power) or (math.max(target_hp - attack_power, 0) / target_max_hp <= 0.20)) then
				rn.spell.cast(me, "zombie_devour")
				return true
			end
		end
		return rn.mods.basegame.prefabs.melee_monster.on_collision(me, other)
	end,
	on_cast_begin = rn.mods.basegame.prefabs.melee_monster.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.melee_monster.on_death,
	on_equip = rn.mods.basegame.prefabs.melee_monster.on_equip,
	on_unequip = rn.mods.basegame.prefabs.melee_monster.on_unequip,
}