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
	melee_1h_horizontal = 5,
	melee_unarmed_lunge = 6
}

rn._impl_cast_type_to_animation_name =
{
	"Cast1H_Directed",
	"Cast1H_Omni",
	"Cast2H_Directed",
	"Cast2H_Omni",
	"Attack1H_Horizontal",
	"ZombiePunch"
}

rn._impl_cast_type_to_animation_id =
{
	2,
	3,
	4,
	5,
	1,
	11
}

-- Include all abilities here.
require "ability0"
require "ability1"
require "ability2"
require "ability3"
require "ability4"
require "ability5"
require "ability6"
require "ability7"
require "ability8"

rn.cast_spell = function(arg)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Cast Spell")
	obj:set_text("Spell: " .. arg.ability_name)
	-- check argument sanity
	local ent = arg.ent
	tz.assert(ent ~= nil)
	local ability_name = arg.ability_name
	tz.assert(ability_name ~= nil)

	-- figure out which ability we wanna cast
	local ability = rn.abilities[rn.ability.type[ability_name]]
	tz.assert(ability ~= nil)

	local face_cast_direction = arg.face_cast_direction
	-- should the entity face the direction of the cast?
	if face_cast_direction == nil then
		face_cast_direction = false
	end

	local instant_cast_override = arg.instant_cast_override
	if instant_cast_override == nil then
		instant_cast_override = false
	end

	local cast_type = ability.cast_type
	if cast_type == nil then
		cast_type = rn.cast.type.spell_1h_directed
	end

	if arg.cast_type_override ~= nil then
		cast_type = arg.cast_type_override
	end

	-- if we're casting something else, early-out.
	local entdata = rn.entity_get_data(ent)
	local casting = rn.entity_data_read(ent, "impl.is_casting")
	if casting == true then return end

	entdata.impl.face_cast_direction = face_cast_direction
	-- if its an instant cast spell, no need to set these, just send it instantly. (note: no animation in this case)
	if ability.base_cast_time == 0 or instant_cast_override then
		-- just instantly send it
		rn.entity_data_write(ent, "impl.cast", ability_name)
		rn.complete_cast(ent)
		return
	end

	rn.entity_data_write(ent, "impl.cast", ability_name, "impl.cast_begin", tz.time(), "impl.is_casting", true)

	ent:get_element():play_animation_by_name(rn._impl_cast_type_to_animation_name[cast_type], false)

	-- casting effect
	entdata.impl.cast_effects = {nil, nil}
	-- right hand = 1, left hand = 2
	tz.assert(ability.magic_type ~= nil)
	local has_magic_visual = ability.magic_type ~= "Physical"
	if has_magic_visual then
		entdata.impl.cast_effects[1] = rn.scene():get(rn.scene():add(4))
		local rhdata = rn.entity.data[entdata.impl.cast_effects[1]:uid()]
		rhdata.target_entity = ent
		rhdata.subobject = 21
		-- play flipbook 2 times per cast.
		rhdata.cast_duration = ability.base_cast_time * 0.5
		local r, g, b = rn.damage_type_get_colour(ability.magic_type)
		rhdata.colour_r = r
		rhdata.colour_g = g
		rhdata.colour_b = b

		if ability.dual_wield_cast then
			entdata.impl.cast_effects[2] = rn.scene():get(rn.scene():add(4))
			local lhdata = rn.entity.data[entdata.impl.cast_effects[2]:uid()]
			lhdata.target_entity = ent
			lhdata.subobject = 17
			-- play flipbook 2 times per cast.
			lhdata.cast_duration = ability.base_cast_time * 0.5
			lhdata.colour_r = r
			lhdata.colour_g = g
			lhdata.colour_b = b
		end
	end
end

rn.complete_cast = function(ent)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Complete Cast")
	tz.assert(ent:is_valid())
	local entdata = rn.entity_get_data(ent)
	local cast = rn.entity_data_read(ent, "impl.cast")
	local ability = rn.abilities[rn.ability.type[cast]]

	obj:set_text("Cast - " .. cast)

	tz.assert(ability ~= nil)
	ability.on_cast(ent)
	rn.cancel_cast(ent)
end

rn.cancel_cast = function(ent)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Cancel Cast")
	local entdata = rn.entity_get_data(ent)
	rn.entity_data_write(ent, "impl.cast", nil, "impl.cast_begin", nil, "impl.is_casting", false)
	if entdata.impl.cast_effects ~= nil then
		for i=1,2,1 do
			if entdata.impl.cast_effects[i] ~= nil then
				rn.scene():remove_uid(entdata.impl.cast_effects[i]:uid())
				entdata.impl.cast_effects[i] = nil
			end
		end
	end
end

rn.casting_advance = function(ent)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Casting Advance")
	-- entity is currently casting a spell.
	local entdata = rn.entity_get_data(ent)
	local cast, cast_begin = rn.entity_data_read(ent, "impl.cast", "impl.cast_begin")
	tz.assert(cast ~= nil)
	obj:set_text("Casting Advance - " .. cast)
	local ability = rn.abilities[rn.ability.type[cast]]
	tz.assert(ability ~= nil)
	local t = tz.time()
	if t > (cast_begin + ability.base_cast_time) then
		rn.complete_cast(ent)
	end

	if entdata.impl.face_cast_direction then
		local e = ent:get_element()
		local vecx, vecy = rn.entity_data_read(ent, "impl.cast_dir_x", "impl.cast_dir_y")
		vecx = vecx or 0
		vecy = vecy or 0
		local dir = nil
		if(vecx < 0.0) then
			-- could be left
			if math.abs(vecx) > math.abs(vecy) then
				-- definitely right
				e:face_right()
				dir = "right"
			else
				-- abs(vecx) <= abs(vecy)
				-- meaning could be up or down
				if vecy >= 0.0 then
					e:face_forward()
					dir = "forward"
				else
					e:face_backward()
					dir = "backward"
				end
			end
		elseif vecx > 0.0 then
			-- could be left
			if vecx > math.abs(vecy) then
				-- definitely left
				e:face_left()
				dir = "left"
			else
				if vecy >= 0.0 then
					e:face_forward()
					dir = "forward"
				else
					e:face_backward()
					dir = "backward"
				end
			end
		else
			-- vecx == 0.0
			if vecy >= 0.0 then
				e:face_forward()
				dir = "forward"
			else
				e:face_backward()
				dir = "backward"
			end
		end
		rn.entity_data_write(ent, "impl.dir", dir)
	end
end

rn.is_casting = function(ent)
	return rn.entity_data_read(ent, "impl.is_casting") == true
end

rn.get_current_cast = function(ent)
	if rn.is_casting(ent) then
		return rn.entity_data_read(ent, "impl.cast")
	end
	return nil
end

rn.get_ability_id = function(name)
	return rn.ability.type[name]
end

rn.get_ability_name = function(id)
	return rn.abilities[id].name
end