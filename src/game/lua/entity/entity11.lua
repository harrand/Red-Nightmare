local id = 11
local typestr = "banshee"
rn.entity.type[typestr] = id

rn.entity_handler[id] =
{
	-- invoked exactly once during game initialisation.
	-- if this entity has any unique bespoke resources to pre-load, now is the time.
	static_init = function()
		tracy.ZoneBeginN(typestr .. " - static init")
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity0.png")
		tracy.ZoneEnd()
	end,
	preinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - preinit")
		ent:set_name("Banshee")
		ent:set_model(rn.model.humanoid)

		local bstats = ent:get_base_stats()
		bstats:set_maximum_health(100)
		bstats:set_attack_power(10)
		bstats:set_spell_power(10)
		bstats:set_movement_speed(0.5)
		bstats:set_defence_rating(10)
		ent:set_base_stats(bstats)
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(3, 0, texh)
		ent:get_element():object_set_texture_tint(3, 0, 0.1, 0.3, 0.3)

		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		ent:set_faction(rn.faction_id.player_enemy)

		local data = rn.entity_get_data(ent)
		data.fireball_cd = 10

		tracy.ZoneEnd()
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		if ent:is_dead() then
			return
		end

		data.fireball_cd = data.fireball_cd - rn.delta_time

		if data.target == nil then
			for i=1,rn.scene():size()-1,1 do
				-- attempt to find a new enemy to chase.
				local ent2 = rn.scene():get(i)	
				if not ent:is_dead() and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" then
					data.target = ent2
				end
			end
		else
			if data.target:is_dead() or not data.target:is_valid() then
				-- set target to nil so we choose one.
				data.target = nil
			end
		end

		if rn.is_casting(ent) and data.target ~= nil then
			local tarx, tary = data.target:get_element():get_position()
			--local mousex, mousey = rn.scene():get_mouse_position_ws()
			local entx, enty
			if ent:get_model() == rn.model.humanoid then
				entx, enty = ent:get_element():get_subobject_position(21)
			else
				entx, enty = ent:get_element():get_position()
			end
			-- we want vector, so mouse pos - ent pos
			local vecx = entx - tarx
			local vecy = enty - tary
			data.impl.cast_dir_x = vecx
			data.impl.cast_dir_y = vecy
		end

		if data.fireball_cd <= 0.0 and data.target ~= nil then
			rn.cast_spell({ent = ent, ability_name = "Frostbolt", face_cast_direction = true})
			data.fireball_cd = 4
		end

		-- attempt to attack any enemy nearby
		rn.for_each_collision(ent, function(ent2)
			if not ent:is_dead() and not data.collided_this_update and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" then
				data.collided_this_update = true
				rn.cast_spell({ent = ent, ability_name = "Melee", cast_type_override = rn.cast.type.melee_unarmed_lunge})
			end
		end)
		if not rn.is_casting(ent) and not ent:is_dead() then
			if data.target ~= nil then
				rn.entity_move_to_entity({ent = ent, movement_anim_name = "Run"}, data.target)
			else
				-- otherwise just move right forever???
				-- todo: wander around aimlessly
				rn.entity_move{ent = ent, dir = "right", movement_anim_name = "Run"}
			end
		end
	end
}