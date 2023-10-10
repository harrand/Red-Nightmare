local id = 1
local typestr = "block_darkstone"
rn.entity.type[typestr] = id
rn.entity.data[id] =
{
	flipbook_timer = 0,
	cur_texture_id = 0
}
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		for i=0,3,1 do
			tm:register_texture(typestr .. ".sprite" .. i, "./res/images/sprites/magic_ball/magic_ball" .. i .. ".png")
		end
	end,
	preinit = function(ent)
		ent:set_name("Darkstone Block")
		ent:set_model(rn.model.quad)
	end,
	postinit = function(ent)
		ent:get_element():face_right()
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(1, 0, texh)
		ent:get_element():object_set_texture_tint(1, 0, 1.0, 0.35, 0.05)
	end,
	update = function(ent)
		tz.assert(ent:get_name() == "Darkstone Block")
		local data = rn.entity.data[id]
		data.flipbook_timer = data.flipbook_timer + rn.delta_time
		-- when flipbook timer hits a threshold (fps / 4), advance to the next frame
		if data.flipbook_timer > 0.1 then
			data.flipbook_timer = 0
			data.cur_texture_id = (data.cur_texture_id + 1) % 4
			local texh = rn.texture_manager():get_texture(typestr .. ".sprite" .. data.cur_texture_id)
			ent:get_element():object_set_texture_handle(1, 0, texh)
		end
	end
}