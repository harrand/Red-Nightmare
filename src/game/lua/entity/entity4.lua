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

		rn.entity_data_write(ent, "spawned_at", tz.time())
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)

		rn.entity_data_write(ent, "impl.targetable", false, "impl.projectile_skip", true, "impl.light", rn.scene():add_light())
	end,
	deinit = function(ent)
		rn.scene():remove_light(rn.entity_data_read(ent, "impl.light"))
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		local target_entity_uid, subobject, cast_duration, magic_type, spawned_at = rn.entity_data_read(ent, "target_entity", "subobject", "cast_duration", "magic_type", "spawned_at")
		tz.assert(target_entity_uid ~= nil and type(target_entity_uid) == 'number')
		local target_entity = rn.scene():get_uid(target_entity_uid)
		local r, g, b = rn.damage_type_get_colour(magic_type)
		ent:get_element():object_set_texture_tint(2, 0, r, g, b)
		tz.assert(subobject ~= nil)
		local xtar, ytar = target_entity:get_element():get_subobject_position(subobject)
		ent:get_element():set_position(xtar, ytar)

		local light = rn.scene():get_light(rn.entity_data_read(ent, "impl.light"))
		light:set_position(xtar, ytar)
		light:set_colour(r, g, b)

		local casted_time = tz.time() - spawned_at
		local cast_progress = casted_time / cast_duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7
		light:set_power(0.7 * cast_progress)
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
	end
}