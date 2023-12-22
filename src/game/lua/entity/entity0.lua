local id = 0
local typestr = "player_lady_melistra"
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
		ent:set_name("Lady Melistra")
		ent:set_model(rn.model.humanoid)

		local bstats = ent:get_base_stats()
		bstats:set_maximum_health(100)
		bstats:set_attack_power(0)
		bstats:set_spell_power(25)
		bstats:set_defence_rating(10)
		ent:set_base_stats(bstats)
		local data = rn.entity_get_data(ent)
		data.impl.custom_despawn_timer = -1
		data.mouse_suppressed = false
		data.keyboard_suppressed = false
		data.impl.drop_items_on_death = false
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(3, 0, texh)

		rn.equip(ent, "White Cloth Headband")

		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		ent:set_faction(rn.faction_id.player_ally)

		tracy.ZoneEnd()
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		tz.assert(ent:get_name() == "Lady Melistra")
		if ent:is_dead() then
			return
		end

		if rn.is_casting(ent) then
			local mousex, mousey = rn.scene():get_mouse_position_ws()
			local entx, enty
			if ent:get_model() == rn.model.humanoid then
				entx, enty = ent:get_element():get_subobject_position(21)
			else
				entx, enty = ent:get_element():get_position()
			end
			-- we want vector, so mouse pos - ent pos
			local vecx = entx - mousex
			local vecy = enty - mousey
			data.impl.cast_dir_x = vecx
			data.impl.cast_dir_y = vecy
		end
		
		local wnd = tz.window()
		if not rn.is_casting(ent) and wnd:is_mouse_down("left") and not data.mouse_suppressed then
			rn.cast_spell({ent = ent, ability_name = "Fireball", face_cast_direction = true})
		elseif not rn.is_casting(ent) and wnd:is_mouse_down("right") and not data.mouse_suppressed then
			--rn.cast_spell({ent = ent, ability_name = "Allure of Flame", face_cast_direction = true})
			rn.cast_spell({ent = ent, ability_name = "Consecrate"})
			--rn.cast_spell({ent = ent, ability_name = "Melee", face_cast_direction = true})
		elseif not rn.is_casting(ent) and wnd:is_mouse_down("middle") and not data.mouse_suppressed then
			--rn.cast_spell({ent = ent, ability_name = "Touch of Death", face_cast_direction = true})
			rn.cast_spell({ent = ent, ability_name = "Frozen Orb", face_cast_direction = true})
		end

		dir = {}
		if rn.is_key_down("w") and not data.keyboard_suppressed then
			table.insert(dir, "backward")
		end
		if rn.is_key_down("s") and not data.keyboard_suppressed then
			table.insert(dir, "forward")
		end
		if rn.is_key_down("a") and not data.keyboard_suppressed then
			table.insert(dir, "left")
		end
		if rn.is_key_down("d") and not data.keyboard_suppressed then
			table.insert(dir, "right")
		end
		if data.direction_force ~= nil then
			dir = {data.direction_force}
		end
		if not rawequal(next(dir), nil) then
			rn.entity_move({ent = ent, dir = dir, movement_anim_name = "Run"})
		end

		if rn.is_key_down("esc") and rn.is_casting(ent) and not data.keyboard_suppressed then
			rn.cancel_cast(ent)
		end
	end,
	on_kill = function(ent, evt)
		-- player killed someone
		local data = rn.entity_get_data(ent)
		if data.impl.exp == nil then
			data.impl.exp = 0
		end
		local killed_thing = rn.scene():get_uid(evt.damagee)
		--print(killed_thing:get_name() .. " dies, " .. ent:get_name() .. " gains " .. tostring(killed_thing:get_level()) .. " exp.")
		data.impl.exp = data.impl.exp + killed_thing:get_level()
		if data.impl.exp >= (ent:get_level() * 4) + 16 then
			-- level up!
			print("DING!")
			rn.play_sound("level_up.mp3")
			data.impl.exp = 0
			ent:level_up()
		end

		rn.player_credits = rn.player_credits + killed_thing:get_level() * killed_thing:get_type() * 2
	end,
	on_death = function(ent, evt)
		rn.director.paused = true
		rn.game_over()
		rn.play_sound("player_death.mp3")
		rn.stop_music(0)
		rn.play_music("soundtrack_sad.mp3", 0, 0.8)
	end,
	on_struck = function(ent, evt)
		if ent:get_health() > evt.value then
			rn.play_sound("player_hurt.mp3")
		end
	end
}