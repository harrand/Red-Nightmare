local id = 7
local typestr = "barrier"
-- intended to be used to display dropped items on the floor.
-- for each dropped item, you can equip it onto this mannequin and it will display as if being worn by an invisible humanoid.
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		tm:register_texture(typestr .. ".sprite", "./res/images/sprites/magic_barrier.png")
	end,
	preinit = function(ent)
		ent:set_name("Magic Barrier")
		ent:set_model(rn.model.quad)
		ent:set_collideable(false)

		rn.entity.data[ent:uid()] =
		{
			spawned_at = tz.time(),
			target_entity = nil,
			colour_r = 1.0,
			colour_g = 1.0,
			colour_b = 1.0,
		}
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".sprite")
		ent:get_element():object_set_texture_handle(2, 0, texh)
	end,
	update = function(ent)
		local data = rn.entity.data[ent:uid()]
		tz.assert(data.target_entity ~= nil)
		ent:get_element():object_set_texture_tint(2, 0, data.colour_r, data.colour_g, data.colour_b)
		local xtar, ytar = data.target_entity:get_element():get_subobject_position(7)
		ytar = ytar + 7.5 * data.target_entity:get_element():get_uniform_scale()
		ent:get_element():set_position(xtar, ytar)


		local casted_time = tz.time() - data.spawned_at
		local cast_progress = casted_time / data.duration
		local average_size = 1.25
		local pct_time_till_full_growth = 0.02
		if cast_progress < pct_time_till_full_growth then
			ent:get_element():set_uniform_scale(average_size * (1.0 / pct_time_till_full_growth) * cast_progress)
			return
		end
		-- scale should vary between avg-0.05 and avg+0.05
		ent:get_element():set_uniform_scale(average_size + 0.05 * math.sin(cast_progress * 250.0))
		if cast_progress > 0.8 then
			-- quickly flicker the effect on-and-off to signify the effect ends soon.
			ent:get_element():object_set_visibility(2, not ent:get_element():object_get_visibility(2))	
		end
		if cast_progress > 1 then
			rn.scene():remove_uid(ent:uid())
		end
	end
}
