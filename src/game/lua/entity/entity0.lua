local id = 0
local typestr = "player_lady_melistra"
rn.entity.type[typestr] = id

function keep_playing_animation(e, anim_id, loop)
	if e:get_playing_animation_id() ~= anim_id or not e:is_animation_playing() then
		e:play_animation(anim_id, loop)
	end
end

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
		ent:set_base_stats(bstats)
		tracy.ZoneEnd()
	end,
	postinit = function(ent)
		tracy.ZoneBeginN(typestr .. " - postinit")
		tz.assert(rn.texture_manager():has_texture(typestr .. ".skin"))
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		rn.equip(ent, "Iron Sallet")

		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)
		ent:set_faction(rn.faction_id.player_ally)

		tracy.ZoneEnd()
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		tz.assert(ent:get_name() == "Lady Melistra")

		if rn.is_casting(ent) then
			local mousex, mousey = rn.scene():get_mouse_position_ws()
			local entx, enty = ent:get_element():get_position()
			-- we want vector, so mouse pos - ent pos
			local vecx = entx - mousex
			local vecy = enty - mousey
			local w, h = tz.window():get_dimensions()
			local aspect_ratio = w / h
			data.impl.cast_dir_x = vecx * aspect_ratio
			data.impl.cast_dir_y = vecy
		end
		
		local wnd = tz.window()
		if not rn.is_casting(ent) and wnd:is_mouse_down("left") then
			rn.cast_spell({ent = ent, ability_name = "Fireball", face_cast_direction = true})
		elseif not rn.is_casting(ent) and wnd:is_mouse_down("right") then
			rn.cast_spell({ent = ent, ability_name = "Melee", face_cast_direction = true})
		end

		if rn.is_key_down("w") then
			rn.entity_move({ent = ent, dir = "backward", movement_anim_id = 8})
		elseif rn.is_key_down("s") then
			rn.entity_move({ent = ent, dir = "forward", movement_anim_id = 8})
		elseif rn.is_key_down("a") then
			rn.entity_move({ent = ent, dir = "left", movement_anim_id = 8})
		elseif rn.is_key_down("d") then
			rn.entity_move({ent = ent, dir = "right", movement_anim_id = 8})
		end

		if rn.is_key_down("esc") and rn.is_casting(ent) then
			rn.cancel_cast(ent)
		end
	end
}