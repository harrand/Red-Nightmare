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
		for i=1,2,1 do
			local light_id = rn.scene():add_light()
			local light = rn.scene():get_light(light_id)
			light:set_power(0.8)
			rn.entity_data_write(ent, "impl.lights[" .. i .. "]", light_id)
		end
		-- attempted optimisation. unpack might not work this way.
		rn.entity_data_write(ent, "magic_type", "Frost", "impl.drop_items_on_death", false)

		rn.equip(ent, "Lightning Crown")

		tracy.ZoneEnd()
	end,
	on_struck = function(ent, evt)
		local damager = rn.scene():get_uid(evt.damager)
		rn.entity_get_data(ent).target = damager
	end,
	on_death = function(ent)
		local data = rn.entity_get_data(ent)
		for i=1,2,1 do
			local light_id = rn.entity_data_read(ent, "impl.lights[" .. i .. "]")
			if light_id ~= nil then
				rn.scene():remove_light(light_id)
			end
		end
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		if ent:is_dead() then
			return
		end

		local magic_type = rn.entity_data_read(ent, "magic_type")
		local r, g, b = rn.damage_type_get_colour(magic_type)
		r = r * 2
		g = g * 2
		b = b * 2
		ent:get_element():object_set_texture_tint(3, 0, r, g, b)
		for i=1,2,1 do
			local light_id = rn.entity_data_read(ent, "impl.lights[" .. i .. "]")
			tz.assert(light_id ~= nil)
			local light = rn.scene():get_light(light_id)
			light:set_colour(r, g, b)
		end

		local light1, light2 = rn.entity_data_read(ent, "impl.lights[1]", "impl.lights[2]")
		rn.scene():get_light(light1):set_position(ent:get_element():get_subobject_position(21))
		rn.scene():get_light(light2):set_position(ent:get_element():get_subobject_position(17))

		data.fireball_cd = data.fireball_cd - rn.delta_time

		aggro_range = 25
		local target_args = {aggro_range = aggro_range, target_relationship = "hostile"}
		if data.target == nil then
			data.target = rn.entity_target_entity(ent, target_args)
		else
			if not rn.impl_entity_entity_valid_target(ent, target_args, data.target) then
				data.target = nil
			end
		end

		if rn.is_casting(ent) and data.target ~= nil then
			tz.assert(data.target:is_valid())
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
			rn.entity_data_write(ent, "impl.cast_dir_x", vecx, "impl.cast_dir_y", vecy)
		end

		if data.fireball_cd <= 0.0 and data.target ~= nil then
			local spell_name = nil
			if magic_type == "Fire" then
				spell_name = "Fireball"
			elseif magic_type == "Shadow" or magic_type == "Anthir" then
				spell_name = "Shadowbolt"
			elseif magic_type == "Frost" then
				spell_name = "Frostbolt"
			end
			rn.cast_spell({ent = ent, ability_name = spell_name, face_cast_direction = true})
			data.fireball_cd = 4
			data.collided_this_second = false
		end

		local health_pct = ent:get_health() / ent:get_stats():get_maximum_health()
		-- fire elemental: cast fiery detonation when 20% hp or under.
		if magic_type == "Fire" and not rn.is_casting(ent) and health_pct <= 0.2 then
			rn.cast_spell({ent = ent, ability_name = "Fiery Detonation"})
		end

		-- attempt to attack any enemy nearby
		rn.for_each_collision(ent, function(ent2)
			if not ent:is_dead() and not data.collided_this_second and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_data_read(ent2, "impl.projectile_skip") ~= true then
				-- deal magic damage to colliding enemies
				data.collided_this_second = true
				local evt = rn.entity_damage_entity_event:new()
				evt.damager = ent:uid()
				evt.damagee = ent2:uid()
				evt.value = ent:get_stats():get_spell_power() * 0.2
				evt.damage_type = magic_type
				rn.combat.process_event(evt)
			end
		end)
		if not rn.is_casting(ent) and not ent:is_dead() then
			if data.target ~= nil then
				rn.entity_move_to_entity({ent = ent, movement_anim_name = "Run"}, data.target)
			else
				-- otherwise just move right forever???
				-- todo: wander around aimlessly
				--rn.entity_move{ent = ent, dir = "right", movement_anim_name = "Run"}
			end
		end
	end
}