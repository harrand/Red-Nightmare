rn.mods.basegame.levels.devproc0 =
{
	static_init = function()
		rn.renderer():add_texture("background.blackrock", "basegame/res/textures/background_blackrock.png")
		rn.renderer():add_texture("background.blackrock_normals", "basegame/res/textures/background_blackrock_normals.png")
		rn.renderer():add_texture("material.darkstone", "basegame/res/textures/material_darkstone.png")
		rn.renderer():add_texture("material.darkstone_normals", "basegame/res/textures/material_darkstone_normals.png")
	end,
	on_load = function()
		local player = rn.current_scene():add_entity("player_melistra")
		rn.level.data_write("player", player)

		local wallscale = 4
		local boundx, boundy = rn.renderer():get_view_bounds()
		boundx = math.floor((boundx * 0.5) - wallscale)
		boundy = math.floor((boundy * 0.5) - wallscale)
		for x=-boundx,boundx,wallscale do
			for y=-boundy,boundy,wallscale do
				if x == -boundx or x >= (boundx - wallscale + 1) or y == -boundy or y >= (boundy - wallscale + 1) then
					local ent = rn.current_scene():add_entity("wall")
					rn.entity.prefabs.sprite.set_position(ent, x, y)
					rn.entity.prefabs.sprite.set_scale(ent, wallscale * 0.5)
					rn.entity.prefabs.sprite.set_texture(ent, "material.darkstone")
					rn.entity.prefabs.sprite.set_normal_map(ent, "material.darkstone_normals")
				else
					-- randomly 10%...
					local randval = math.random()
					if randval <= 0.1 then
						-- 2.5% chance of spawning a loot chest
						if randval <= 0.025 then
							local ent = rn.current_scene():add_entity("loot_chest")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
						else
							-- 7.5% chance of just a wall
							local ent = rn.current_scene():add_entity("wall")
							rn.entity.prefabs.sprite.set_texture(ent, "material.darkstone")
							rn.entity.prefabs.sprite.set_normal_map(ent, "material.darkstone_normals")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
							rn.entity.prefabs.sprite.set_scale(ent, wallscale * 0.5)
						end
					end
				end
			end
		end

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, boundx, boundy)
		rn.entity.prefabs.sprite.set_texture(bg, "background.blackrock")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.blackrock_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)

		local main_chest = rn.current_scene():add_entity("loot_chest")
		rn.entity.prefabs.loot_chest.add_loot(main_chest, "steel_longsword")
	end
}