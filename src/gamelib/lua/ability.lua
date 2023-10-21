rn.ability = {}
rn.ability.data = {}
rn.ability.type = {}
rn.abilities = {}

rn.cast = {}
rn.cast.type =
{
	spell_1h_directed = 1,
	spell_1h_omni = 2,
	spell_2h_directed = 3,
	spell_2h_omni = 4,
}

rn._impl_cast_type_to_animation_id =
{
	2,
	3,
	4,
	5
}

-- Include all abilities here.
require "ability0"

rn.cast_spell = function(arg)
	-- check argument sanity
	local ent = arg.ent
	tz.assert(ent ~= nil)
	local ability_name = arg.ability_name
	tz.assert(ability_name ~= nil)
	local cast_type = arg.type
	if cast_type == nil then cast_type = rn.cast.type.spell_1h_directed end

	-- figure out which ability we wanna cast
	local ability = rn.abilities[rn.ability.type[ability_name]]
	tz.assert(ability ~= nil)

	-- if we're casting something else, early-out.
	local entdata = rn.entity_get_data(ent)
	if entdata.impl.is_casting == true then return end

	-- if its an instant cast spell, no need to set these, just send it instantly. (note: no animation in this case)
	if ability.base_cast_time == 0 then
		-- just instantly send it
		ability.on_cast(ent)
		return
	end

	entdata.impl.is_casting = true
	entdata.impl.cast_begin = tz.time()
	entdata.impl.cast = ability_name
	print(ent:get_name() .. " - BEGAN CASTING " .. ability_name)

	ent:get_element():play_animation(rn._impl_cast_type_to_animation_id[cast_type], false)
end

rn.casting_advance = function(ent)
	-- entity is currently casting a spell.
	local entdata = rn.entity_get_data(ent)
	tz.assert(entdata.impl.cast ~= nil)
	local ability = rn.abilities[rn.ability.type[entdata.impl.cast]]
	tz.assert(ability ~= nil)
	local t = tz.time()
	if t > (entdata.impl.cast_begin + ability.base_cast_time) then
		-- cast finished!
		ability.on_cast(ent)
		entdata.impl.is_casting = false
		entdata.impl.cast_begin = nil
		print(ent:get_name() .. " - FINISHED CASTING " .. entdata.impl.cast)
		entdata.impl.cast = nil
	end
end

rn.is_casting = function(ent)
	return rn.entity_get_data(ent).impl.is_casting == true
end

rn.get_current_cast = function(ent)
	if rn.is_casting(ent) then return rn.entity_get_data(ent).impl.cast end
	return nil
end