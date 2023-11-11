local id = 3
local typestr = "Divine Barrier"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1250,
	magic_type = "Divine",
	cast_type = rn.cast.type.spell_1h_omni,
	on_cast = function(ent)
		local barrier = rn.scene():get(rn.scene():add(7))
		local barrierdata = rn.entity_get_data(barrier)
		barrierdata.target_entity = ent
		barrierdata.duration = 10000
		local ability = rn.abilities[id]
		barrierdata.colour_r = ability.magic_colour_r
		barrierdata.colour_g = ability.magic_colour_g
		barrierdata.colour_b = ability.magic_colour_b

		local immune_buff = rn.new_buff()
		immune_buff:set_time_remaining(barrierdata.duration / 1000)
		immune_buff:set_increased_defence_rating(9999999999)
		immune_buff:set_name(typestr)
		ent:apply_buff(immune_buff)
	end
}