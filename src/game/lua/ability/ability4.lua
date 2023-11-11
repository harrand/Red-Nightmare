local id = 4
local typestr = "Allure of Flame"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1250,
	magic_type = "Fire",
	cast_type = rn.cast.type.spell_2h_omni,
	dual_wield_cast = true,
	on_cast = function(ent)
		local nova = rn.scene():get(rn.scene():add(10))
		nova:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		local x2, y2 = ent:get_element():get_subobject_position(17)
		x = (x + x2) / 2.0
		y = (y + y2) / 2.0
		nova:get_element():set_position(x, y)
		local entdata = rn.entity_get_data(ent)
		local novadata = rn.entity_get_data(nova)
		-- fireball base stats is a snapshot of the caster's current stats.
		nova:set_base_stats(ent:get_stats())
		novadata.owner = ent

		local mousex, mousey = rn.scene():get_mouse_position_ws()	
		local bx, by = ent:get_element():get_position()
		local vecx = mousex - bx
		local vecy = mousey - by
		local hypot = math.sqrt(vecx*vecx + vecy*vecy)
		if hypot > 0.0 then
			vecx = vecx / hypot
			vecy = vecy / hypot
			-- maximum length of min(hypot, MY_MAX)
			local dist_travel = math.min(hypot, 10)
			local destx = bx + (dist_travel * vecx)
			local desty = by + (dist_travel * vecy)
			ent:get_element():set_position(destx, desty)
		end
	end
}