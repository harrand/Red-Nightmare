local id = 1
local typestr = "Fireball"
rn.ability.type[typestr] = id

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1000,
	magic_colour_r = 1.0,
	magic_colour_g = 0.35,
	magic_colour_b = 0.05,
	cast_type = rn.cast.type.spell_1h_directed,
	on_cast = function(ent)
		print("FIREBALL GO WOOSH!")
		local sc = rn.scene()
		local proj = sc:get(sc:add(1))
		proj:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		proj:get_element():set_position(x, y)
		local entdata = rn.entity_get_data(ent)
		local projdata = rn.entity_get_data(proj)
		projdata.shoot_direct = entdata.impl.face_cast_direction
		if projdata.shoot_direct then
			-- shoot directly in the cast direction
			projdata.shoot_vec_x = entdata.impl.cast_dir_x
			projdata.shoot_vec_y = entdata.impl.cast_dir_y
			-- math.atan2 is removed, instead just use math.atan with 2 args
			proj:get_element():rotate(3.14159 + math.atan(projdata.shoot_vec_y, projdata.shoot_vec_x))
		else
			-- shoot in the general direction (right/left/up/down)
			projdata.shoot_dir = entdata.impl.dir
			if projdata.shoot_dir == "right" then
				-- do nothing
			elseif projdata.shoot_dir == "left" then
				proj:get_element():rotate(3.14159)
			elseif projdata.shoot_dir == "forward" then
				proj:get_element():rotate(-1.5708)
			elseif projdata.shoot_dir == "backward" then
				proj:get_element():rotate(1.5708)
			else
				tz.assert(false)
			end
		end

		local ability = rn.abilities[id]
		projdata.colour_r = ability.magic_colour_r
		projdata.colour_g = ability.magic_colour_g
		projdata.colour_b = ability.magic_colour_b
	end
}