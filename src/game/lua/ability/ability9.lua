local id = 9
local typestr = "Reanimate"
rn.ability.type[typestr] = id
-- entity heals itself for 100% of spell power

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 2500,
	magic_type = "Physical",
	cast_type = rn.cast.type.none,
	on_cast = function(ent)
		ent:set_health(ent:get_stats():get_maximum_health())
		local entdata = rn.entity_get_data(ent)
		entdata.impl.death_time = nil
		entdata.impl.undead_immune = false

		if ent:get_type() == 13 then
			-- its an elemental. set its type to our magic type.
			entdata.magic_type = "Anthir"
			-- turn all its lights back on too!
			for i=1,2,1 do
				entdata.impl.lights[i] = rn.scene():add_light()
				entdata.impl.lights[i]:set_power(0.8)
			end
		else
			-- undead things despawn very fast.
			entdata.impl.undead = true
			entdata.impl.custom_despawn_timer = 5000
		end
	end
}