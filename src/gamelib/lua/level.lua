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
		local fgdata = rn.entity_get_data(rn.level_fg)

		bgdata.dynamic_texture_scale = true
		bgdata.texture_scale_zoom = 16.0
		bgdata.impl.targetable = false
		bgdata.impl.projectile_skip = true
		rn.level_bg:get_element():set_uniform_scale(64)
		rn.level_bg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blanchfield_cemetary.background"))
		rn.level_bg:get_element():object_set_texture_handle(2, 1, rn.texture_manager():get_texture("blanchfield_cemetary.background_normals"))
		rn.level_bg:get_element():set_depth(-2.5)

		rn.level_fg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blanchfield_cemetary.foreground"))
		rn.level_fg:get_element():set_uniform_scale(64)
		rn.level_fg:get_element():set_depth(-2)
		fgdata.impl.targetable = false
		fgdata.impl.projectile_skip = true
	end

	if args.name == "blackrock_dungeon" or args.name == "startscreen" then
		rn.level_bg = rn.scene():get(rn.scene():add(8))

		--rn.scene():get_renderer():set_ambient_light(0.15, 0.2, 0.4) -- snowy
		rn.scene():get_renderer():set_ambient_light(0.5, 0.5, 0.5)

		local bgdata = rn.entity_get_data(rn.level_bg)

		bgdata.dynamic_texture_scale = true
		bgdata.texture_scale_zoom = 16.0
		bgdata.impl.targetable = false
		bgdata.impl.projectile_skip = true
		rn.level_bg:get_element():set_uniform_scale(64)
		rn.level_bg:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("blackrock_dungeon.background"))
		rn.level_bg:get_element():object_set_texture_handle(2, 1, rn.texture_manager():get_texture("blackrock_dungeon.background_normals"))
		rn.level_bg:get_element():set_depth(-2.5)
	end

	if args.name ~= "startscreen" then
		rn.director.restart()
	end
	print("Loading level " .. args.name)
	rn.scene():get_renderer():clear_strings()
	if rn.player == nil then
		rn.player = rn.scene():get(rn.scene():add(0))
	else
		rn.player:get_element():set_position(0, 0)
	end
	local pd = rn.entity_get_data(rn.player)
	rn.entity_get_data(rn.player).keyboard_suppressed = false
	rn.entity_get_data(rn.player).mouse_suppressed = false
	rn.entity_get_data(rn.player).direction_force = nil

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

require "startscreen"
require "level0"
require "level1"