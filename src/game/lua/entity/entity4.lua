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
		ent:set_collideable(false)

		rn.entity.data[ent:uid()] =
		{
			flipbook_timer = 0,
			cur_texture_id = 0,
			spawned_at = tz.time(),
			target_entity = nil,
			cast_duration = nil,
			subobject = nil,
			reverse = false,
			colour_r = 1.0,
			colour_g = 1.0,
			colour_b = 1.0
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		local data = rn.entity_get_data(ent)
		data.impl.light = rn.scene():add_light()
		data.impl.projectile_skip = true
		rn.entity_data_write(ent, "impl.targetable", false)
	end,
	deinit = function(ent)
		rn.scene():remove_light(rn.entity_get_data(ent).impl.light)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		ent:get_element():object_set_texture_tint(2, 0, data.colour_r, data.colour_g, data.colour_b)
		tz.assert(data.target_entity ~= nil)
		tz.assert(data.subobject ~= nil)
		local xtar, ytar = data.target_entity:get_element():get_subobject_position(data.subobject)
		ent:get_element():set_position(xtar, ytar)

		data.impl.light:set_position(xtar, ytar)
		data.impl.light:set_colour(data.colour_r, data.colour_g, data.colour_b)

		local casted_time = tz.time() - data.spawned_at
		local cast_progress = casted_time / data.cast_duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7
		if data.reverse then
			frame_id = 6 - frame_id
		end
		data.impl.light:set_power(0.7 * cast_progress)
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
	end
}