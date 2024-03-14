rn.mods.basegame.items.fiery_hauberk =
{
	tooltip = [[
		+100% more Fire Power
		Equip: Every second, instantly shoot a Lesser Fireball at a nearby enemy.
	]],
	static_init = function()
		rn.renderer():add_texture("chest.platebody", "basegame/res/skins/chests/platebody.png")
	end,
	on_equip = function(uuid)
		if not rn.entity.prefabs.combat_stats.is_alive(uuid) then
			return
		end
		rn.entity.prefabs.light_emitter.instantiate(uuid)

		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, 20)
		rn.entity.prefabs.combat_stats.apply_pct_more_fire_power(uuid, 2.0)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.light_emitter.on_remove(uuid)

		rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(uuid, -20)
		rn.entity.prefabs.combat_stats.apply_pct_more_fire_power(uuid, 0.5)
	end,
	on_update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		if not rn.entity.prefabs.combat_stats.is_alive(uuid) then
			return
		end
		local t = sc:entity_read(uuid, "fiery_hauberk_timer") or 0.0
		t = t + delta_seconds

		local fireball_period = 0.5
		if t > fireball_period then
			local x, y, z_ = sc:entity_get_local_position(uuid)
			local proj = rn.spell.spells.lesser_firebolt.finish(uuid, x, y)
			if proj == nil then return end
			local dx = math.random(-1, 1)
			local dy = math.random(-1, 1)
			rn.entity.prefabs.magic_ball_base.set_target(proj, dx * 999, dy * 999)
			t = 0
		end

		sc:entity_write(uuid, "fiery_hauberk_timer", t)


		local colour = rn.spell.schools.fire.colour
		rn.entity.prefabs.light_emitter.set_colour(uuid, colour[1], colour[2], colour[3])
		rn.entity.prefabs.light_emitter.set_power(uuid, 2.0 * math.abs(math.sin(t) / fireball_period) + 0.1)
		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
	end,
	slot = rn.item.slot.chest,
	colour = rn.spell.schools.fire.colour,
	rarity = "legendary",
	texture = "chest.chainmail",
}