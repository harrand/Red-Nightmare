rn.mods.basegame.prefabs.elemental_base =
{
	description = "An elemental!",
	static_init = function()
		rn.renderer():add_texture("skin.just_eyes", "basegame/res/skins/just_eyes.png")
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.bipedal.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.bipedal_human.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_can_equip(uuid, true)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.just_eyes")
		rn.entity.prefabs.bipedal.set_run_animation(uuid, "Idle")
		rn.entity.prefabs.light_emitter.instantiate(uuid)
		--rn.entity.prefabs.keyboard_controlled.bind_spell(uuid, 1, "melee")
		--rn.entity.prefabs.bipedal.set_run_animation(uuid, "ZombieWalk")

		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, 5)
		rn.entity.prefabs.combat_stats.set_base_physical_power(uuid, 2.0)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed * 0.9)
		rn.entity.prefabs.combat_stats.set_base_haste(uuid, -0.25)

		rn.entity.prefabs.ranged_ai.instantiate(uuid)
		rn.entity.prefabs.ranged_ai.set_aggro_range(uuid, 25)
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
		rn.entity.prefabs.ranged_ai.update(uuid, delta_seconds)

		local magic_type = rn.current_scene():entity_read(uuid, ".elemental_magic_type")
		if magic_type ~= nil then
			local col = rn.spell.schools[magic_type].colour
			rn.entity.prefabs.light_emitter.set_colour(uuid, col[1], col[2], col[3])
			rn.entity.prefabs.light_emitter.set_power(uuid, 1.25)
			rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
			rn.entity.prefabs.bipedal.set_colour(uuid, col[1], col[2], col[3])
		end
	end,
	on_struck = rn.mods.basegame.prefabs.ranged_ai.on_struck,
	on_move = rn.mods.basegame.prefabs.bipedal.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.bipedal.on_stop_moving,
	on_collision = function(me, other)
		-- return true if obstacle, false otherwise
		-- i.e we dont collide with anything aside from obstacle
		return rn.entity.prefabs.obstacle.is_obstacle(other)
	end,
	on_cast_begin = rn.mods.basegame.prefabs.bipedal.on_cast_begin,
	on_cast_success = rn.mods.basegame.prefabs.ranged_ai.on_cast_success,
	on_death = function(uuid, dmg, magic_type, enemy_uuid)
		rn.entity.prefabs.light_emitter.on_remove(uuid)
		rn.mods.basegame.prefabs.bipedal.on_death(uuid, dmg, magic_type, enemy_uuid)
	end,
	on_equip = rn.mods.basegame.prefabs.bipedal.on_equip,
	on_unequip = rn.mods.basegame.prefabs.bipedal.on_unequip,
}

for schoolname, schooldata in pairs(rn.spell.schools) do
	if schoolname ~= "physical" then
		-- is a magic school.
		rn.mods.basegame.prefabs[schoolname .. "_elemental"] =
		{
			description = "An elemental comprised of " .. schoolname,
			pre_instantiate = rn.mods.basegame.prefabs.elemental_base.pre_instantiate,
			instantiate = function(uuid)
				rn.entity.prefabs.elemental_base.instantiate(uuid)
				rn.entity.prefabs.spell_slots.equip_spell(uuid, "lesser_" .. schoolname .. "bolt")
				rn.item.equip(uuid, schoolname .. "_elemental_hood")
				rn.item.equip(uuid, schoolname .. "_elemental_robe")
				rn.current_scene():entity_write(uuid, ".elemental_magic_type", schoolname)
			end,
			update = rn.mods.basegame.prefabs.elemental_base.update,
			on_collision = rn.mods.basegame.prefabs.elemental_base.on_collision,
			on_struck = rn.mods.basegame.prefabs.elemental_base.on_struck,
			on_move = rn.mods.basegame.prefabs.elemental_base.on_move,
			on_stop_moving = rn.mods.basegame.prefabs.elemental_base.on_stop_moving,
			on_cast_begin = rn.mods.basegame.prefabs.elemental_base.on_cast_begin,
			on_cast_success = rn.mods.basegame.prefabs.elemental_base.on_cast_success,
			on_death = function(uuid, dmg, magic_type, enemy_uuid)
				rn.entity.prefabs.elemental_base.on_death(uuid, dmg, magic_type, enemy_uuid)
				local x, y = rn.entity.prefabs.sprite.get_position(uuid)
				if schoolname == "frost" then
					rn.spell.drop_at(x, y, "icicle")
				else
					rn.spell.drop_at(x, y, "lesser_" .. schoolname .. "bolt")
				end
			end,
			on_equip= rn.mods.basegame.prefabs.elemental_base.on_equip,
			on_unequip = rn.mods.basegame.prefabs.elemental_base.on_unequip,
			on_remove = rn.mods.basegame.prefabs.elemental_base.on_remove
		}
	end
end