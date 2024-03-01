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
	holy = 
	{
		pretty_name = "Holy",
		colour = {1.0, 0.85, 0.4},
	}
}

rn.spell.cast = function(uuid, spell_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Spell Cast - \"" .. spell_name .. "\"")
	local sc = rn.current_scene()
	if rn.spell.is_casting(uuid) then return end

	-- begin cast.
	sc:entity_write(uuid, "cast.name", spell_name)
	sc:entity_write(uuid, "cast.begin", tz.time())
	rn.spell.create_effect_on(uuid, spell_name)
	rn.entity.on_cast_begin(uuid, spell_name)

	local spelldata = rn.spell.spells[spell_name]
	if spelldata ~= nil then
		if spelldata.on_cast_begin ~= nil then
			spelldata.on_cast_begin(uuid)
		end
	end
end

rn.spell.is_casting = function(uuid)
	return rn.current_scene():entity_read(uuid, "cast.name") ~= nil
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

	local base_cast_time = spelldata.cast_duration
	local haste = rn.entity.prefabs.combat_stats.get_haste(uuid) or 0.0
	-- new_casting_time = base_casting_time / (1 + haste)
	local cast_time = base_cast_time / (1.0 + haste)
	local is_instant_cast = cast_time == nil or cast_time == 0.0
	local cast_completed = tz.time() > (spell_began + (cast_time or 0.0) * 1000.0)
	if is_instant_cast or cast_completed then


		-- the spell has finished casting. unleash its effect.
		-- firstly, where are we casting?
		-- if we specified a location uuid, get its position now:
		local castx = nil
		local casty = nil
		local cast_location = sc:entity_read(uuid, "cast.location")
		if cast_location ~= nil then
			-- location is an internal variable name. a pointer if you will.
			local location_uuid = sc:entity_read(uuid, cast_location)
			-- we expect it to be a sprite.
			castx, casty = rn.entity.prefabs.sprite.get_position(location_uuid)
		else
			-- we dont know. in truth we should be screwed here.
			-- however add a fallback just incase - the caster's root global position.
			-- its rarely what we want, but sometimes is (and will often be near enough rather than crashing here.)
			castx, casty = rn.entity.prefabs.sprite.get_position(uuid)
		end
		rn.entity.on_cast_success(uuid)
		if spelldata.finish ~= nil then
			spelldata.finish(uuid, castx, casty)
		end
		-- then cleanup
		rn.spell.clear(uuid)
		-- then return - we dont invoke advance on the frame the spell completes.
		return
	end

	local cast_elapsed = tz.time() - spell_began
	local cast_progress = cast_elapsed / (cast_time * 1000.0)

	if spelldata.advance ~= nil then
		spelldata.advance(uuid, cast_progress)
	end
end

rn.spell.clear = function(uuid)
	local sc = rn.current_scene()
	sc:entity_write(uuid, "cast.name", nil)
	sc:entity_write(uuid, "cast.begin", nil)
	sc:entity_write(uuid, "cast.location", nil)
	rn.spell.clear_effect_on(uuid)
	rn.entity.on_cast_end(uuid)
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
		sc:entity_write(uuid, "cast.location", "cast_buildup0")
		-- stick the effect to the caster
		rn.entity.prefabs.sticky.stick_to(effect, uuid)
	elseif model == "bipedal" then
		-- do left hand:
		local effect = sc:add_entity("cast_buildup")
		sc:entity_write(effect, "magic_type", magic_type)
		sc:entity_write(uuid, "cast_buildup0", effect)
		sc:entity_write(uuid, "cast.location", "cast_buildup0")
		-- stick the effect to the caster's left hand
		rn.entity.prefabs.sticky.stick_to_subobject(effect, uuid, rn.entity.prefabs.bipedal.right_hand)
		-- do right hand if spell is marked as two-handed:
		if spelldata.two_handed == true then
			local effect2 = sc:add_entity("cast_buildup")
			sc:entity_write(effect2, "magic_type", magic_type)
			sc:entity_write(uuid, "cast_buildup1", effect2)
			sc:entity_write(uuid, "cast.location", "cast_buildup0")
			-- stick the effect to the caster's right hand
			rn.entity.prefabs.sticky.stick_to_subobject(effect2, uuid, rn.entity.prefabs.bipedal.left_hand)
		end
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
	elseif model == "bipedal" then
		local buildup0 = sc:entity_read(uuid, "cast_buildup0")
		if buildup0 ~= nil and sc:contains_entity(buildup0) then
			-- delete the buildup effect coz we're done casting.
			sc:remove_entity(buildup0)
		end
		-- buildup1 will exist if the cast was two-handed (i.e there was a spell effect on the right hand too.)
		local buildup1 = sc:entity_read(uuid, "cast_buildup1")
		if buildup1 ~= nil and sc:contains_entity(buildup1) then
			-- delete the buildup effect coz we're done casting.
			sc:remove_entity(buildup1)
		end
	else
		tz.error(false, "No support for model " .. tostring(model) .. " having cast effects.")
	end
end