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
		local old_flat_inc = prfb["get_flat_increased_" .. stat_name](uuid) or 0.0
		prfb["set_flat_increased_" .. stat_name](uuid, old_flat_inc + val)
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
		local old_pct_inc = prfb["get_pct_increased_" .. stat_name](uuid) or 0.0
		prfb["set_pct_increased_" .. stat_name](uuid, old_pct_inc + val)
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
		local old_pct_more = prfb["get_pct_more_" .. stat_name](uuid) or 0.0
		prfb["set_pct_more_" .. stat_name](uuid, 1.0 - ((1.0 + old_pct_more) * (1.0 + val)))
	end

	-- get the stat value, after all modifiers.
	prfb["get_" .. stat_name] = function(uuid)
		local base = prfb["get_base_" .. stat_name](uuid)
		local flat_inc = prfb["get_flat_increased_" .. stat_name](uuid)
		local pct_inc = prfb["get_pct_increased_" .. stat_name](uuid)
		local pct_more = prfb["get_pct_more_" .. stat_name](uuid)
		--The basic damage calculation is:
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
	dmg_unmit = function(uuid, dmg)
		tz.assert(dmg >= 0.0, "Damage dealt cannot be negative.")
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		local max_hp = rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid) or 0.0
		if minus_hp >= max_hp then
			-- bloke is already dead. dont bother doing anything.
			return
		end
		-- clamp minus_hp between 0 and max_hp
		minus_hp = math.min(math.max(minus_hp + dmg, 0.0), max_hp)
		if minus_hp >= max_hp then
			-- man finna die.
		end
		rn.current_scene():entity_write(uuid, "hp_lost", minus_hp)
	end,
	heal_unmit = function(uuid, heal)
		tz.assert(heal >= 0.0, "Healing received can't be negative.")
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		if minus_hp == 0 then
			-- bloke is already full hp.
			return
		end
		minus_hp = math.max(0.0, minus_hp - heal)
		rn.current_scene():entity_write(uuid, "hp_lost", minus_hp)
	end,
	is_dead = function(uuid)
		local minus_hp = rn.current_scene():entity_read(uuid, "hp_lost") or 0.0
		local max_hp = rn.mods.basegame.prefabs.combat_stats.get_max_hp(uuid) or 0.0
		local invincible = rn.current_scene():entity_read(uuid, "invincible") or false
		if invincible then return false end
		return minus_hp >= max_hp
	end,
	dmg = function(uuid, dmg, magic_type, enemy_uuid)
		-- firstly:
		-- get our resistance
		local our_resistance = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_resist"](uuid)
		-- get the caster's power of the given magic type
		local caster_power = 0.0
		if enemy_uuid ~= 0 then
			caster_power = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_power"](enemy_uuid)
		end
		
		-- dmg = (base_dmg * caster_power) * max(our_resistance - 1.0, 0.0)
		local mitigated_dmg = (dmg * (caster_power + 1.0)) * math.max(1.0 - our_resistance, 0.0)
		rn.entity.prefabs.combat_stats.dmg_unmit(uuid, mitigated_dmg)

		-- todo: formal combat logging
		print(rn.current_scene():entity_get_name(uuid) .. " took " .. mitigated_dmg .. " " .. magic_type .. " damage from " .. rn.current_scene():entity_get_name(enemy_uuid))
	end
}

rn_impl_create_combat_stat("max_hp")
for schoolname, schooldata in pairs(rn.spell.schools) do
	rn_impl_create_combat_stat(schoolname .. "_power")
	rn_impl_create_combat_stat(schoolname .. "_resist")
end

-- magic-type powers and resistances