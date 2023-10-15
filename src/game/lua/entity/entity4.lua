local id = 4
local typestr = "cast_buildup"
-- intended to be used to display dropped items on the floor.
-- for each dropped item, you can equip it onto this mannequin and it will display as if being worn by an invisible humanoid.
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		for i=0,6,1 do
			tm:register_texture(typestr .. ".sprite" .. i, "./res/images/sprites/cast_buildup/cast_buildup" .. i .. ".png")
		end
	end,
	preinit = function(ent)
		ent:set_name("Cast Buildup")
		ent:set_model(rn.model.quad)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			spawned_at = tz.time(),
			target_entity = nil,
			cast_duration = nil,
			subobject = nil
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(1, 0, texh)
		ent:get_element():object_set_texture_tint(1, 0, 1.0, 0.35, 0.05)
		ent:get_element():face_right()
		ent:get_element():rotate(-1.5708)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		tz.assert(data.target_entity ~= nil)
		tz.assert(data.subobject ~= nil)
		local xtar, ytar = data.target_entity:get_element():get_subobject_position(data.subobject)
		ent:get_element():set_position(xtar, ytar)

		local casted_time = tz.time() - data.spawned_at
		local cast_progress = casted_time / data.cast_duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7
		print("frame id = " .. frame_id)
		ent:get_element():object_set_texture_handle(1, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
	end
}