local id = 12
local typestr = "loot container interactable"
rn.entity.type[typestr] = id
rn.entity_handler[id] =
{
	static_init = function()
		local tm = rn.texture_manager()
		--tm:register_texture("grassy.background", "./res/images/scenery/backgrounds/background_grassy.png")
		tm:register_texture("chest.basic0", "./res/images/sprites/chest/chest0.png")
		tm:register_texture("chest.basic0_normals", "./res/images/sprites/chest/chest0_normals.png")
	end,
	preinit = function(ent)
		ent:set_collideable(true)
		ent:set_immoveable(true)
		ent:set_model(rn.model.quad)
		ent:set_faction(rn.faction_id.player_enemy)
		local stats = ent:get_base_stats()
		stats:set_maximum_health(1)
		ent:set_base_stats(stats)
	end,
	on_death = function(ent)
		-- todo: chest open animation
		ent:get_element():object_set_visibility(2, false)
		local data = rn.entity_get_data(ent)
		if data.loot ~= nil then
			local x, y = ent:get_element():get_position()
			rn.drop_item_at(data.loot, x, y)
		else
			tz.report("no loot. sadgers :(")
		end
	end,
	postinit = function(ent)
		ent:set_name("Chest")
		ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("chest.basic0"))
		ent:get_element():object_set_texture_handle(2, 1, rn.texture_manager():get_texture("chest.basic0_normals"))
		ent:get_element():object_set_texture_tint(2, 0, 1, 1, 1);
		ent:get_element():set_uniform_scale(0.75)
	end
}
