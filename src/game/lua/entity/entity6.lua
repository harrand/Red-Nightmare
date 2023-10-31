local id = 6
local typestr = "blood splatter"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		for i=0,6,1 do
			tm:register_texture(typestr .. ".sprite" .. i, "./res/images/sprites/blood_splatter/blood_splatter" .. i .. ".png")
		end
	end,
	preinit = function(ent)
		ent:set_name("Blood Splatter")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			spawned_at = tz.time(),
			target_entity = nil,
			duration = nil,
			subobject = nil
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		ent:get_element():face_right()
		ent:get_element():rotate(-1.5708)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		ent:get_element():object_set_texture_tint(2, 0, 0.85, 0.1, 0.1)
		tz.assert(data.target_entity ~= nil)
		local xtar, ytar = data.target_entity:get_element():get_subobject_position(data.subobject)
		ent:get_element():set_position(xtar, ytar)

		local casted_time = tz.time() - data.spawned_at
		local cast_progress = casted_time / data.duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7
		if data.reverse then
			frame_id = 6 - frame_id
		end
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
		if cast_progress > 1 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}