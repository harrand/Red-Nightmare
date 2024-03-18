rn.mods.basegame.prefabs.melee_monster =
{
	static_init = function()
		rn.renderer():add_texture("skin.man", "basegame/res/skins/man.png")
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.bipedal.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_can_equip(uuid, true)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.man")

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 20)
		rn.entity.prefabs.combat_stats.set_base_physical_power(uuid, 2.0)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed)

		rn.entity.prefabs.melee_ai.instantiate(uuid)
		rn.entity.prefabs.melee_ai.set_aggro_range(uuid, 15)
	end,
	update = function(uuid, delta_seconds)
		if rn.entity.prefabs.combat_stats.is_dead(uuid) then
			-- i know in theory on_death should handle this.
			-- however, let's say youre moving in the same frame you die.
			-- its possible the "run" animation message is sent after another entity hits you and kills you on another worker thread.
			-- for that reason, if we accidentally overplay the death animation in this edge-case, we re-play it here.
			local playing_anim = rn.current_scene():entity_get_playing_animation(uuid)
			local death_anim = rn.entity.prefabs.bipedal.get_death_animation(uuid)
			if playing_anim ~= nil and playing_anim ~= death_anim then
				rn.current_scene():entity_play_animation(uuid, death_anim)
			end
			return
		end
		rn.entity.prefabs.bipedal.update(uuid, delta_seconds)
		rn.entity.prefabs.melee_ai.update(uuid, delta_seconds)
	end,
	on_move = rn.mods.basegame.prefabs.bipedal.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.bipedal.on_stop_moving,
	on_collision = rn.mods.basegame.prefabs.melee_ai.on_collision,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.bipedal.on_death,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
}