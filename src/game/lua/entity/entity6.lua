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
		ent:set_collideable(false)

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
		local data = rn.entity_get_data(ent)
		rn.entity_data_write(ent, "impl.targetable", false)
		data.impl.projectile_skip = true
	end,
	deinit = function(ent)
		local data = rn.entity_get_data(ent)
		if data.impl.light ~= nil then
			rn.scene():remove_light(data.impl.light)
		end
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]

		if data.damage_type ~= "Physical" and data.impl.light == nil then
			-- spawn our light -.-
			local r, g, b = rn.damage_type_get_colour(data.damage_type)
			data.impl.light = rn.scene():add_light()
			data.impl.light:set_power(1.0)
			data.impl.light:set_colour(r, g, b)
		end

		ent:get_element():object_set_texture_tint(2, 0, 0.85, 0.1, 0.1)

		if data.target_entity == nil or not data.target_entity:is_valid() then
			rn.scene():remove_uid(ent:uid())
			return
		end
		local xtar, ytar
		if data.subobject == nil then
			xtar, ytar = data.target_entity:get_element():get_position()
		else
			xtar, ytar = data.target_entity:get_element():get_subobject_position(data.subobject)
		end
		ent:get_element():set_position(xtar, ytar)

		local casted_time = tz.time() - data.spawned_at
		local cast_progress = casted_time / data.duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7
		if data.reverse then
			frame_id = 6 - frame_id
		end

		if data.impl.light ~= nil then
			data.impl.light:set_position(xtar, ytar)
			data.impl.light:set_power(math.sqrt(1.0 - cast_progress))
		end
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
		if cast_progress > 1 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}