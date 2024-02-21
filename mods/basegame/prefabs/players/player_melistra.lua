rn.mods.basegame.prefabs.player_melistra =
{
	description = "Lady Melistra, the first playable character in Red Nightmare.",
	static_init = function()
		rn.renderer():add_texture("skin.melistra", "basegame/res/skins/melistra.png")
	end,
	pre_instantiate = function(uuid)
		--return rn.entity.prefabs.sprite.pre_instantiate(uuid)
		return rn.entity.prefabs.bipedal.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
		rn.entity.prefabs.bipedal.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.melistra")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 1, "lesser_firebolt")
		rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 2, "lesser_frostbolt")

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 100)
		rn.entity.prefabs.combat_stats.set_base_fire_power(uuid, 10)
		rn.entity.prefabs.combat_stats.set_base_fire_resist(uuid, 0.1)
	end,
	update = function(uuid, delta_seconds)
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
	on_collision = function(me, other)
		return true
	end,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin
}