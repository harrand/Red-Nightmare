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
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite0")
		ent:get_element():object_set_texture_handle(2, 0, texh)
		rn.entity_data_write(ent, "impl.targetable", false, "impl.projectile_skip", true, "spawned_at", tz.time())
	end,
	deinit = function(ent)
		local light_id = rn.entity_data_read(ent, "impl.light")
		if light_id ~= nil then
			rn.scene():remove_light(light_id)
		end
	end,
	update = function(ent)
		local light_id, damage_type, target_uid, subobject, duration, spawned_at = rn.entity_data_read(ent, "impl.light", "damage_type", "target_entity", "subobject", "duration", "spawned_at")
		local target_entity = nil
		if target_uid ~= nil then
			target_entity = rn.scene():get_uid(target_uid)
		end

		if damage_type ~= "Physical" and light_id == nil then
			-- spawn our light -.-
			local r, g, b = rn.damage_type_get_colour(damage_type)
			light_id = rn.scene():add_light()
			local light = rn.scene():get_light(light_id)
			light:set_power(1.0)
			light:set_colour(r, g, b)
			rn.entity_data_write(ent, "impl.light", light_id)
		end

		ent:get_element():object_set_texture_tint(2, 0, 0.85, 0.1, 0.1)

		if target_entity == nil or not target_entity:is_valid() then
			rn.scene():remove_uid(ent:uid())
			return
		end
		local xtar, ytar
		if subobject == fakenil then
			xtar, ytar = target_entity:get_element():get_position()
		else
			xtar, ytar = target_entity:get_element():get_subobject_position(subobject)
		end
		ent:get_element():set_position(xtar, ytar)

		local casted_time = tz.time() - spawned_at
		local cast_progress = casted_time / duration
		-- frame count = 7
		local frame_id = math.floor(cast_progress * 6) % 7

		if light_id ~= nil then
			local light = rn.scene():get_light(light_id)
			light:set_position(xtar, ytar)
			light:set_power(math.sqrt(1.0 - cast_progress))
		end
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture(typestr .. ".sprite" .. frame_id))
		if cast_progress > 1 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}