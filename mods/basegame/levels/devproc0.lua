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
		rn.renderer():set_ambient_light(0.2, 0.2, 0.225, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 1.0)

		local difficulty = rn.data_store():read("difficulty") or 0
		local wallscale = 4
		local size = 64
		local boundx = size
		local boundy = size
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
						randval = randval / (math.min(difficulty, 20) + 1)
						-- 2.5% chance of spawning a loot chest
						if randval <= 0.025 then
							local rarity = "common"
							if randval <= 0.0001 then
								rarity = "legendary"
							elseif randval <= 0.00085 then
								rarity = "epic"
							elseif randval <= 0.005 then
								rarity = "rare"
							end
							local ent = rn.current_scene():add_entity("loot_chest")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
							rn.entity.prefabs.loot_chest.add_random_loot(ent, rarity)
						else
							-- 7.5% chance of just a wall
							local ent = rn.current_scene():add_entity("wall")
							rn.entity.prefabs.sprite.set_texture(ent, "material.darkstone")
							rn.entity.prefabs.sprite.set_normal_map(ent, "material.darkstone_normals")
							rn.entity.prefabs.sprite.set_position(ent, x, y)
							rn.entity.prefabs.sprite.set_scale(ent, wallscale * 0.5)
						end
					end

					local randval2 = math.random()
					if randval2 <= 0.01 then
						local pn = "zombie"
						if randval2 <= 0.002 then
							pn = "frost_elemental"
						end
						local ent = rn.current_scene():add_entity(pn)
						rn.entity.prefabs.sprite.set_position(ent, x, y)
						-- buff depending on difficulty
						-- health increased by 2 per level
						rn.entity.prefabs.combat_stats.apply_flat_increased_max_hp(ent, difficulty + 2)

						for schoolname, schooldata in pairs(rn.spell.schools) do
							-- all powers increased by 1 per level
							rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_power"](ent, difficulty)
							-- all resistances increased by 0.5 per level
							rn.entity.prefabs.combat_stats["apply_flat_increased_" .. schoolname .. "_resist"](ent, difficulty * 0.005)
						end
					end
					local randval3 = math.random()
					if randval3 <= 0.02 then
						rn.entity.prefabs.weapon_model_torch.spawn_on_ground(x, y)
					end
				end
			end
		end

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, boundx, boundy)
		rn.entity.prefabs.sprite.set_texture(bg, "background.blackrock")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.blackrock_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)

		local reload_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(reload_portal, -boundx + wallscale, boundy - wallscale)
		rn.entity.prefabs.portal.set_colour(reload_portal, 1.0, 0.0, 0.0)
	end
}