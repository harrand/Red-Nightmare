function rn_impl_create_combat_stat(stat_name)
	local prfb = rn.mods.basegame.prefabs.combat_stats
	-- base stat is obvious and exactly what is says on the tin.
	-- e.g "max health: 10"
	prfb["get_base_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "base_" .. stat_name) or 0.0
	end

	prfb["set_base_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "base_" .. stat_name, val)	
	end

	-- flat increased stat is an additive bonus to a stat, before multiplicative modifiers.
	-- e.g "your max health is increased by 50"
	prfb["get_flat_increased_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "flat_inc_" .. stat_name) or 0.0
	end

	prfb["set_flat_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "flat_inc_" .. stat_name, val)
	end

	prfb["apply_flat_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write_add(uuid, "flat_inc_" .. stat_name, val)
	end

	-- additional percentage stat applied. stacks additively
	-- e.g "you have 10% increased maximum health"
	prfb["get_pct_increased_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "pct_inc_" .. stat_name) or 0.0
	end

	prfb["set_pct_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "pct_inc_" .. stat_name, val)
	end

	prfb["apply_pct_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write_add(uuid, "pct_inc_" .. stat_name, val)
	end

	-- additional percentage stat applied. stacks multiplicatively
	-- e.g "you have 10% more maximum health"
	prfb["get_pct_more_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "pct_more_" .. stat_name) or 0.0
	end

	prfb["set_pct_more_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "pct_more_" .. stat_name, val)
	end

	prfb["apply_pct_more_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write_multiply_percentage(uuid, "pct_more_" .. stat_name, val)
	end

	-- get the stat value, after all modifiers.
	prfb["get_" .. stat_name] = function(uuid)
		local base = prfb["get_base_" .. stat_name](uuid)
		local flat_inc = prfb["get_flat_increased_" .. stat_name](uuid)
		local pct_inc = prfb["get_pct_increased_" .. stat_name](uuid)
		local pct_more = prfb["get_pct_more_" .. stat_name](uuid)
		--The basic stat calculation is:
		-- Stat = (Base_Stat + Flat_Inc_Stat) * Pct_Increased_Stat * Pct_More_Stat 
		-- note that pct_inc and pct_more are percentages, so `0.2` more means `1 + 0.2 = 120% more`
		return (base + flat_inc) * (pct_inc + 1.0) * (pct_more + 1.0)
	end
end

rn.mods.basegame.prefabs.combat_stats =
{
	description = "Provides combat stats (e.g hp, magical power, magical resistance etc...) for an entity.",
	instantiate = function(uuid)
	end,
	get_hp = function(uuid)
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		return (rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid) or 0.0) - minus_hp
	end,
	set_hp = function(uuid, hp)
		local max_hp = rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid)
		local hp_lost = math.min(math.max(hp - max_hp, 0), max_hp)
		rn.current_scene():entity_write(uuid, "hp_lost", hp_lost)
	end,
	get_absorb = function(uuid)
		local absorb = rn.current_scene():entity_read(uuid, "absorb") or 0.0
		-- if its gone negative, treat it as zero.
		if absorb < 0.0 then absorb = 0.0 end
		return absorb
	end,
	set_absorb = function(uuid, amt)
		rn.current_scene():entity_write(uuid, "absorb", amt)
	end,
	add_absorb = function(uuid, amt)
		rn.current_scene():entity_write_add(uuid, "absorb", amt)

		if amt > 0.0 then
			rn.entity.prefabs.health_bar.display(uuid, 5.0)

			-- floating combat text
			local sc = rn.current_scene()
			local text = sc:add_entity("floating_combat_text")
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
			rn.entity.prefabs.floating_combat_text.set(text, x, y, "+" .. tostring(math.floor(amt)), {1.0, 0.7, 0.0})
		end
	end,
	dmg_unmit = function(uuid, dmg)
		tz.assert(dmg >= 0.0, "Damage dealt cannot be negative.")
		local just_died = false
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		local max_hp = rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid) or 0.0
		local absorb = rn.mods.basegame.prefabs.combat_stats.get_absorb(uuid)
		if minus_hp >= max_hp then
			-- bloke is already dead. dont bother doing anything.
			return false
		end
		if absorb >= 0.0 then
			local absorbed_dmg = math.min(absorb, dmg)
			absorb = absorb - absorbed_dmg
			rn.mods.basegame.prefabs.combat_stats.set_absorb(uuid, absorb - absorbed_dmg)
			dmg = dmg - absorbed_dmg
		end
		-- clamp minus_hp between 0 and max_hp
		minus_hp = math.min(math.max(minus_hp + dmg, 0.0), max_hp)
		rn.current_scene():entity_write(uuid, "hp_lost", minus_hp)
		return minus_hp >= max_hp
	end,
	full_heal = function(uuid)
		rn.current_scene():entity_write(uuid, "hp_lost", 0)
	end,
	heal_unmit = function(uuid, heal)
		tz.assert(heal >= 0.0, "Healing received can't be negative.")
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		if minus_hp == 0 then
			-- bloke is already full hp.
			return 0.0
		end
		local real_healing = math.min(minus_hp, heal)
		minus_hp = math.max(0.0, minus_hp - heal)
		rn.current_scene():entity_write(uuid, "hp_lost", minus_hp)
		return real_healing
	end,
	is_alive = function(uuid)
		local has_stats = rn.current_scene():entity_read(uuid, "base_max_hp") ~= nil
		return has_stats and not rn.mods.basegame.prefabs.combat_stats.is_dead(uuid)
	end,
	is_dead = function(uuid)
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		local max_hp = rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid) or 0.0
		local invincible = rn.current_scene():entity_read(uuid, "invincible") or false
		if invincible then return false end
		return minus_hp >= max_hp
	end,
	heal = function(uuid, heal, magic_type, ally_uuid)
		local effective_healing = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_damage"](ally_uuid, heal)
		local real_healing = rn.entity.prefabs.combat_stats.heal_unmit(uuid, effective_healing)

		-- display health bar on healee
		-- for 5 seconds.
		if real_healing > 0.0 then
			rn.entity.prefabs.health_bar.display(uuid, 5.0)

			-- floating combat text
			local sc = rn.current_scene()
			local text = sc:add_entity("floating_combat_text")
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
			rn.entity.prefabs.floating_combat_text.set(text, x, y, tostring(math.ceil(real_healing)), {0.0, 1.0, 0.0})
		end

	end,
	dmg = function(uuid, dmg, magic_type, enemy_uuid)
		-- firstly:
		-- get our resistance
		local our_resistance = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_resist"](uuid)
		-- get the caster's power of the given magic type
		local effective_dmg = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_damage"](enemy_uuid, dmg)
		local mitigated_dmg = effective_dmg * math.max(1.0 - our_resistance, 0.0)
		mitigated_dmg = rn.entity.on_struck(uuid, enemy_uuid, mitigated_dmg, magic_type)
		mitigated_dmg = rn.entity.on_hit(enemy_uuid, uuid, mitigated_dmg, magic_type)
		local just_died = rn.entity.prefabs.combat_stats.dmg_unmit(uuid, mitigated_dmg)
		if just_died then
			rn.entity.on_death(uuid, mitigated_dmg, magic_type, enemy_uuid)
		end

		-- todo: formal combat logging
		print(rn.current_scene():entity_get_name(uuid) .. " took " .. mitigated_dmg .. " " .. magic_type .. " damage from " .. rn.current_scene():entity_get_name(enemy_uuid))

		if mitigated_dmg > 0.0 then
			-- floating combat text
			local sc = rn.current_scene()
			local text = sc:add_entity("floating_combat_text")
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
			rn.entity.prefabs.floating_combat_text.set(text, x, y, tostring(math.ceil(mitigated_dmg)), rn.spell.schools[magic_type].colour)
			
			-- display health bar on damagee
			-- for 5 seconds.
			rn.entity.prefabs.health_bar.display(uuid, 5.0)
		end
	end
}

rn_impl_create_combat_stat("max_hp")
rn_impl_create_combat_stat("movement_speed")
rn_impl_create_combat_stat("haste")
for schoolname, schooldata in pairs(rn.spell.schools) do
	rn_impl_create_combat_stat(schoolname .. "_power")
	rn_impl_create_combat_stat(schoolname .. "_resist")

	local prfb = rn.mods.basegame.prefabs.combat_stats
	prfb["get_" .. schoolname .. "_damage"] = function(uuid, base_dmg)
		local base = prfb["get_base_" .. schoolname .. "_power"](uuid)
		local flat_inc = prfb["get_flat_increased_" .. schoolname .. "_power"](uuid)
		local pct_inc = prfb["get_pct_increased_" .. schoolname .. "_power"](uuid)
		local pct_more = prfb["get_pct_more_" .. schoolname .. "_power"](uuid)
		--The basic damage calculation is:
		-- Stat = (base_damage + Base_Stat + Flat_Inc_Stat) * Pct_Increased_Stat * Pct_More_Stat 
		-- note that pct_inc and pct_more are percentages, so `0.2` more means `1 + 0.2 = 120% more`
		return (base_dmg + base + flat_inc) * (pct_inc + 1.0) * (pct_more + 1.0)
	end
end

-- magic-type powers and resistances