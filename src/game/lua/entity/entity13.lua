local id = 13
local typestr = "elemental"
rn.entity.type[typestr] = id

rn.entity_handler[id] =
{
	-- invoked exactly once during game initialisation.
	-- if this entity has any unique bespoke resources to pre-load, now is the time.
	static_init = function()
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity13.png")
	end,
	preinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - preinit")
		ent:set_name("Elemental")
		ent:set_model(rn.model.humanoid)

		local bstats = ent:get_base_stats()
		bstats:set_maximum_health(100)
		bstats:set_attack_power(10)
		bstats:set_spell_power(10)
		bstats:set_movement_speed(3.0)
		bstats:set_defence_rating(10)
		ent:set_base_stats(bstats)
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(3, 0, texh)
		--ent:get_element():object_set_visibility(3, false)

		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		ent:set_faction(rn.faction_id.player_enemy)

		local data = rn.entity_get_data(ent)
		data.fireball_cd = 1

		-- elemental has 6 lights.
		-- torso
		-- helm
		-- right hand
		-- left hand
		-- right foot
		-- left foot
		data.impl.lights = {}
		data.magic_type = "Frost"
		for i=1,2,1 do
			data.impl.lights[i] = rn.scene():add_light()
			data.impl.lights[i]:set_power(0.8)
		end

		rn.equip(ent, "Lightning Crown")

		tracy.ZoneEnd()
	end,
	on_death = function(ent)
		local data = rn.entity_get_data(ent)
		for i=1,2,1 do
			if data.impl.lights[i] ~= nil then
				rn.scene():remove_light(data.impl.lights[i])
				data.impl.lights[i] = nil
			end
		end
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		if ent:is_dead() then
			return
		end

		local r, g, b = rn.damage_type_get_colour(data.magic_type)
		r = r * 2
		g = g * 2
		b = b * 2
		ent:get_element():object_set_texture_tint(3, 0, r, g, b)
		for i=1,2,1 do
			data.impl.lights[i]:set_colour(r, g, b)
		end

		data.impl.lights[1]:set_position(ent:get_element():get_subobject_position(21))
		data.impl.lights[2]:set_position(ent:get_element():get_subobject_position(17))

		data.fireball_cd = data.fireball_cd - rn.delta_time

		if data.target == nil then
			for i=1,rn.scene():size()-1,1 do
				-- attempt to find a new enemy to chase.
				local ent2 = rn.scene():get(i)	
				if not ent:is_dead() and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_get_data(ent2).impl.targetable ~= false then
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
			local spell_name = "Frostbolt"
			if data.magic_type == "Fire" then
				spell_name = "Fireball"
			end
			rn.cast_spell({ent = ent, ability_name = spell_name, face_cast_direction = true})
			data.fireball_cd = 4
			data.collided_this_second = false
		end

		-- attempt to attack any enemy nearby
		rn.for_each_collision(ent, function(ent2)
			if not ent:is_dead() and not data.collided_this_second and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_get_data(ent2).impl.projectile_skip ~= true then
				-- deal magic damage to colliding enemies
				data.collided_this_second = true
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = ent:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power() * 0.2
				evt.damage_type = data.magic_type
				rn.combat.process_event(evt)
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