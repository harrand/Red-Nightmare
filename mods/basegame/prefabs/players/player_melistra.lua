rn.mods.basegame.prefabs.player_melistra =
{
	description = "Lady Melistra, the first playable character in Red Nightmare.",
	static_init = function()
		rn.renderer():add_texture("skin.melistra", "basegame/res/skins/melistra.png")
		--rn.renderer():add_texture("helm.full", "basegame/res/skins/helms/full_helm.png")
	end,
	pre_instantiate = function(uuid)
		--return rn.entity.prefabs.sprite.pre_instantiate(uuid)
		return rn.entity.prefabs.bipedal.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.melistra")
		--rn.entity.prefabs.bipedal.set_helm_visible(uuid, true)
		--rn.entity.prefabs.bipedal.set_helm_texture(uuid, "helm.full")
		--rn.entity.prefabs.bipedal.set_helm_colour(uuid, 0.6, 0.1, 0.2)
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 1, "lesser_firebolt")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 2, "lesser_frostbolt")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 3, "flash_of_light")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 4, "melee")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 5, "firestorm")

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 20)
		rn.entity.prefabs.combat_stats.set_base_fire_power(uuid, 10)
		rn.entity.prefabs.combat_stats.set_base_fire_resist(uuid, 0.1)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed)
	end,
	update = function(uuid, delta_seconds)
		if rn.entity.prefabs.combat_stats.is_dead(uuid) then
			-- i know in theory on_death should handle this.
			-- however, let's say youre moving in the same frame you die.
			-- its possible the "run" animation message is sent after another entity hits you and kills you on another worker thread.
			-- for that reason, if we accidentally overplay the death animation in this edge-case, we re-play it here.
			local playing_anim = rn.current_scene():entity_get_playing_animation(uuid)
			if playing_anim ~= nil and playing_anim ~= "Death" then
				rn.current_scene():entity_play_animation(uuid, "Death")
			end
			return
		end
		rn.entity.prefabs.bipedal.update(uuid, delta_seconds)
		rn.entity.prefabs.keyboard_controlled.update(uuid, delta_seconds)

		-- face towards mouse position.
		if rn.spell.is_casting(uuid) then
			local mx, my = rn.current_scene():get_mouse_position()
			local x, y = rn.current_scene():entity_get_global_position(uuid)
			rn.entity.prefabs.bipedal.face_direction(uuid, x - mx, y - my)
		end
	end,
	on_move = rn.mods.basegame.prefabs.bipedal.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.bipedal.on_stop_moving,
	on_collision = function(me, other)
		return true
	end,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.bipedal.on_death,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
}