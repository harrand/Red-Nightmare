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
require "ability1"

rn.cast_spell = function(arg)
	-- check argument sanity
	local ent = arg.ent
	tz.assert(ent ~= nil)
	local ability_name = arg.ability_name
	tz.assert(ability_name ~= nil)

	-- figure out which ability we wanna cast
	local ability = rn.abilities[rn.ability.type[ability_name]]
	tz.assert(ability ~= nil)

	local cast_type = ability.cast_type
	if cast_type == nil then
		cast_type = rn.cast.type.spell_1h_directed
	end

	-- if we're casting something else, early-out.
	local entdata = rn.entity_get_data(ent)
	if entdata.impl.is_casting == true then return end

	-- if its an instant cast spell, no need to set these, just send it instantly. (note: no animation in this case)
	if ability.base_cast_time == 0 then
		-- just instantly send it
		rn.complete_cast(ent)
		return
	end

	entdata.impl.is_casting = true
	entdata.impl.cast_begin = tz.time()
	entdata.impl.cast = ability_name
	print(ent:get_name() .. " - BEGAN CASTING " .. ability_name)

	ent:get_element():play_animation(rn._impl_cast_type_to_animation_id[cast_type], false)

	-- casting effect
	entdata.impl.cast_effects = {nil, nil}
	-- right hand = 1, left hand = 2
	if true then
		entdata.impl.cast_effects[1] = rn.scene():get(rn.scene():add(4))
		local rhdata = rn.entity.data[entdata.impl.cast_effects[1]:uid()]
		rhdata.target_entity = ent
		rhdata.subobject = 21
		-- play flipbook 2 times per cast.
		rhdata.cast_duration = ability.base_cast_time * 0.5
		rhdata.colour_r = ability.magic_colour_r
		rhdata.colour_g = ability.magic_colour_g
		rhdata.colour_b = ability.magic_colour_b
	end
end

rn.complete_cast = function(ent)
	local entdata = rn.entity_get_data(ent)
	local ability = rn.abilities[rn.ability.type[entdata.impl.cast]]
	tz.assert(ability ~= nil)
	ability.on_cast(ent)
	print(ent:get_name() .. " - FINISHED CASTING " .. entdata.impl.cast)
	rn.cancel_cast(ent)
end

rn.cancel_cast = function(ent)
	local entdata = rn.entity_get_data(ent)
	entdata.impl.is_casting = false
	entdata.impl.cast_begin = nil
	entdata.impl.cast = nil
	for i=1,2,1 do
		if entdata.impl.cast_effects[i] ~= nil then
			rn.scene():remove_uid(entdata.impl.cast_effects[i]:uid())
			entdata.impl.cast_effects[i] = nil
		end
	end
end

rn.casting_advance = function(ent)
	-- entity is currently casting a spell.
	local entdata = rn.entity_get_data(ent)
	tz.assert(entdata.impl.cast ~= nil)
	local ability = rn.abilities[rn.ability.type[entdata.impl.cast]]
	tz.assert(ability ~= nil)
	local t = tz.time()
	if t > (entdata.impl.cast_begin + ability.base_cast_time) then
		rn.complete_cast(ent)
	end
end

rn.is_casting = function(ent)
	return rn.entity_get_data(ent).impl.is_casting == true
end

rn.get_current_cast = function(ent)
	if rn.is_casting(ent) then return rn.entity_get_data(ent).impl.cast end
	return nil
end

rn.get_ability_id = function(name)
	return rn.ability.type[name]
end

rn.get_ability_name = function(id)
	return rn.abilities[id].name
end