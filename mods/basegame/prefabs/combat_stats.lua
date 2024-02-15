function rn_impl_create_combat_stat(stat_name)
	local prfb = rn.mods.basegame.prefabs.combat_stats
	prfb["get_base_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "base_" .. stat_name) or 0.0
	end

	prfb["set_base_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "base_" .. stat_name, val)	
	end

	prfb["get_flat_increased_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "flat_inc_" .. stat_name) or 0.0
	end

	prfb["set_flat_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "flat_inc_" .. stat_name, val)
	end

	prfb["get_pct_increased_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "pct_inc_" .. stat_name) or 0.0
	end

	prfb["set_pct_increased_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "pct_inc_" .. stat_name, val)
	end

	prfb["get_pct_more_" .. stat_name] = function(uuid)
		return rn.current_scene():entity_read(uuid, "pct_more_" .. stat_name) or 0.0
	end

	prfb["set_pct_more_" .. stat_name] = function(uuid, val)
		rn.current_scene():entity_write(uuid, "pct_more_" .. stat_name, val)
	end

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
	instantiate = function(uuid)
	end,
}

rn_impl_create_combat_stat("max_health")
for schoolname, schooldata in pairs(rn.spell.schools) do
	rn_impl_create_combat_stat(schoolname .. "_power")
	rn_impl_create_combat_stat(schoolname .. "_resist")
end

-- magic-type powers and resistances