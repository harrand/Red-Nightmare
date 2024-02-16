rn = rn or {}
rn.spell = rn.spell or {}
rn.spell.spells = rn.spell.spells or {}

rn.spell.schools =
{
	physical =
	{
		pretty_name = "Physical",
		colour = {1.0, 1.0, 1.0},
	},
	fire =
	{
		pretty_name = "Fire",
		colour = {0.9, 0.4, 0.1},
	},
	frost =
	{
		pretty_name = "Frost",
		colour = {0.3, 0.6, 1.0},
	},
}

rn.spell.cast = function(uuid, spell_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Spell Cast - \"" .. spell_name .. "\"")
	local sc = rn.current_scene()
	local current_spell_name = sc:entity_read(uuid, "cast.name")
	if current_spell_name ~= nil then
		-- already casting
		--tz.report(tostring(uuid) .. " cannot cast " .. spell_name .. " because it is already casting " .. current_spell_name)
		return
	end

	-- begin cast.
	sc:entity_write(uuid, "cast.name", spell_name)
	sc:entity_write(uuid, "cast.begin", tz.time())
	rn.spell.create_effect_on(uuid, spell_name)
end

rn.spell.advance = function(uuid)
	local sc = rn.current_scene()
	local current_spell_name = sc:entity_read(uuid, "cast.name")
	if current_spell_name == nil then
		return
	end

	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Spell Advance - \"" .. current_spell_name .. "\"")


	local t = tz.time()
	local spell_began = sc:entity_read(uuid, "cast.begin")
	tz.assert(spell_began ~= nil, "cast.begin internal missing on entity " .. tostring(uuid) .. " that is currently casting " .. current_spell_name)
	
	local spelldata = rn.spell.spells[current_spell_name]
	if spelldata == nil then
		tz.error(false, "Unrecognised spell name \"" .. current_spell_name .. "\"")
		return
	end

	local is_instant_cast = spelldata.cast_duration == nil or spelldata.cast_duration == 0.0
	local cast_completed = tz.time() > (spell_began + (spelldata.cast_duration or 0.0) * 1000.0)
	if is_instant_cast or cast_completed then
		-- the spell has finished casting. unleash its effect.
		if spelldata.finish ~= nil then
			spelldata.finish(uuid)
		end
		-- then cleanup
		rn.spell.clear(uuid)
		-- then return - we dont invoke advance on the frame the spell completes.
		return
	end

	if spelldata.advance ~= nil then
		spelldata.advance(uuid)
	end
end

rn.spell.clear = function(uuid)
	local sc = rn.current_scene()
	sc:entity_write(uuid, "cast.name", nil)
	sc:entity_write(uuid, "cast.begin", nil)
	rn.spell.clear_effect_on(uuid)
end

rn.spell.create_effect_on = function(uuid, spell_name)
	local sc = rn.current_scene()

	if spell_name == nil then
		return
	end
	local spelldata = rn.spell.spells[spell_name]
	local magic_type = spelldata.magic_type
	if spelldata == nil or magic_type == nil or magic_type == "physical" then
		return
	end

	local model = sc:entity_get_model(uuid)
	if model == "plane" then
		local effect = sc:add_entity("cast_buildup")
		sc:entity_write(effect, "magic_type", magic_type)
		sc:entity_write(uuid, "cast_buildup0", effect)
		-- stick the effect to the caster
		rn.entity.prefabs.sticky.stick_to(effect, uuid)
	else
		tz.error(false, "No support for model " .. tostring(model) .. " having cast effects.")
	end
end

rn.spell.clear_effect_on = function(uuid)
	local sc = rn.current_scene()
	local model = sc:entity_get_model(uuid)
	if model == "plane" then
		local buildup0 = sc:entity_read(uuid, "cast_buildup0")
		if buildup0 ~= nil and sc:contains_entity(buildup0) then
			-- delete the buildup effect coz we're done casting.
			sc:remove_entity(buildup0)
		end
	else
		tz.error(false, "No support for model " .. tostring(model) .. " having cast effects.")
	end
end