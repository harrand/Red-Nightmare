rn.level = {}
rn.level.type = {}
rn.level_handler = {}

rn.load_level = function(args)
	if args.name == nil then
		args.name = "blanchfield"
	end

	rn.scene():clear_except_players()
	if args.name == "blanchfield" then
		rn.level_bg = rn.scene():get(rn.scene():add(8))
		rn.level_fg = rn.scene():get(rn.scene():add(8))

		--rn.scene():get_renderer():set_ambient_light(0.15, 0.2, 0.4) -- snowy
		rn.scene():get_renderer():set_ambient_light(0.5, 0.5, 0.5)

		local bgdata = rn.entity_get_data(rn.level_bg)

		bgdata.dynamic_texture_scale = true
		bgdata.texture_scale_zoom = 16.0
		rn.entity_data_write(rn.level_bg, "impl.targetable", false, "impl.projectile_skip", true)
		rn.level_bg:get_element():set_uniform_scale(64)
		rn.level_bg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blanchfield_cemetary.background"))
		rn.level_bg:get_element():object_set_texture_handle(2, 1, rn.texture_manager():get_texture("blanchfield_cemetary.background_normals"))
		rn.level_bg:get_element():set_depth(-2.5)

		rn.level_fg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blanchfield_cemetary.foreground"))
		rn.level_fg:get_element():set_uniform_scale(64)
		rn.level_fg:get_element():set_depth(-2)
		rn.entity_data_write(rn.level_fg, "impl.targetable", false, "impl.projectile_skip", true)

		rn.director.restart()

		if rn.player == nil then
			rn.player = rn.scene():get(rn.scene():add(0))
		else
			rn.player:get_element():set_position(0, 0)
		end
	end

	if args.name == "blackrock_dungeon" then
		rn.level_bg = rn.scene():get(rn.scene():add(8))

		--rn.scene():get_renderer():set_ambient_light(0.15, 0.2, 0.4) -- snowy
		rn.scene():get_renderer():set_ambient_light(0.5, 0.5, 0.5)

		local bgdata = rn.entity_get_data(rn.level_bg)

		bgdata.dynamic_texture_scale = true
		bgdata.texture_scale_zoom = 16.0
		rn.entity_data_write(rn.level_bg, "impl.targetable", false, "impl.projectile_skip", true)
		rn.level_bg:get_element():set_uniform_scale(64)
		rn.level_bg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blackrock_dungeon.background"))
		rn.level_bg:get_element():object_set_texture_handle(2, 1, rn.texture_manager():get_texture("blackrock_dungeon.background_normals"))
		rn.level_bg:get_element():set_depth(-2.5)

		rn.director.restart()

		if rn.player == nil then
			rn.player = rn.scene():get(rn.scene():add(0))
		else
			rn.player:get_element():set_position(0, 0)
		end
	end

	local handler = rn.level_handler[rn.level.type[args.name]]
	tz.assert(handler ~= nil)
	if handler.on_load ~= nil then
		handler.on_load()
	end

	rn.old_level_cache = args
end

rn.restart_level = function()
	rn.load_level(rn.old_level_cache)
end

require "level0"
require "level1"