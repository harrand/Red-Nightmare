local id = 8
local typestr = "Touch of Death"
rn.ability.type[typestr] = id
-- entity spawns a fireball that deals 100% of spellpower as damage.

rn.abilities[id] =
{
	name = typestr,
	base_cast_time = 1000,
	magic_type = "Anthir",
	cast_type = rn.cast.type.spell_1h_directed,
	on_cast = function(ent)
		local cast, face_cast_direction = rn.entity_data_read(ent, "impl.cast", "impl.face_cast_direction")
		local ability = rn.abilities[rn.ability.type[cast]]

		local proj = rn.scene():get(rn.scene():add(14))
		proj:set_faction(ent:get_faction())
		local x, y = ent:get_element():get_subobject_position(21)
		proj:get_element():set_position(x, y)
		local projdata = rn.entity_get_data(proj)
		-- fireball base stats is a snapshot of the caster's current stats.
		local bstats = ent:get_stats()
		-- this means it will benefit for example from a spellpower buff at the point of casting (snapshotting)
		-- however it has its own movement speed
		bstats:set_movement_speed(6)
		proj:set_base_stats(bstats)
		rn.entity_data_write(proj, "shoot_direct", face_cast_direction)
		projdata.owner = ent
		if face_cast_direction then
			-- shoot directly in the cast direction
			local shoot_vec_x, shoot_vec_y = rn.entity_data_read(ent, "impl.cast_dir_x", "impl.cast_dir_y")
			rn.entity_data_write(proj, "shoot_vec_x", shoot_vec_x, "shoot_vec_y", shoot_vec_y)
			-- math.atan2 is removed, instead just use math.atan with 2 args
			proj:get_element():rotate(math.atan(shoot_vec_y, shoot_vec_x))
		else
			-- shoot in the general direction (right/left/up/down)
			local shoot_dir = rn.entity_data_read(ent, "impl.dir")
			rn.entity_data_write(proj, "shoot_dir", shoot_dir)
			if shoot_dir == "left" then
				-- do nothing
			elseif shoot_dir == "right" then
				proj:get_element():rotate(3.14159)
			elseif shoot_dir == "forward" then
				proj:get_element():rotate(-1.5708)
			elseif shoot_dir == "backward" then
				proj:get_element():rotate(1.5708)
			else
				tz.assert(false)
			end
		end

		rn.entity_data_write(proj, "magic_type", ability.magic_type)
	end
}