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
		local duration = 10000
		local ability = rn.abilities[id]
		rn.entity_data_write(barrier, "target_entity", ent:uid(), "duration", duration, "magic_type", ability.magic_type)
		--barrierdata.target_entity = ent
		--barrierdata.duration = 10000

		local immune_buff = rn.new_buff()
		immune_buff:set_time_remaining(duration / 1000)
		immune_buff:set_increased_defence_rating(9999999999)
		immune_buff:set_name(typestr)
		ent:apply_buff(immune_buff)
	end
}