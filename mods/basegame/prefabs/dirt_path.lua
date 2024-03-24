rn.mods.basegame.prefabs.dirt_path =
{
	description = "Dirt Path",
	static_init = function()
		rn.renderer():add_texture("sprite.dirt_path.2junc", "basegame/res/sprites/dirt_path/2_way_junction.png")
		rn.renderer():add_texture("sprite.dirt_path.2straight", "basegame/res/sprites/dirt_path/2_way_straight.png")
		rn.renderer():add_texture("sprite.dirt_normals", "basegame/res/sprites/dirt_path/dirt_normals.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_normal_map(uuid, "sprite.dirt_normals")
	end,
	plot_path = function(scale, xbegin, ybegin, path_string)
		-- e.g 0.0, 0.0, r, r, u, u, l, u
		local path_base_scale = 2.0
		local depth = -1.5

		local curx = xbegin
		local cury = ybegin
		local dirold = nil
		for dir in path_string:gmatch(".") do
			-- figure out based on old direction which type of path we want.
			local texture = "sprite.dirt_path.2straight"
			local rotation = 0
			-- if they're both horizontal, then its a straight path
			if dirold == nil or dirold == dir or (dirold == "r" and dir == "l") or (dirold == "l" and dir == "r") or (dirold == "u" and dir == "d") or (dirold == "d" and dir == "u") then
				texture = "sprite.dirt_path.2straight"
				-- if direction is horizontal though we need 90 rotation.
				if dir == "r" or dir == "l" then
					rotation = math.pi * -0.5
				end
			elseif (dirold == "u" and dir == "r") or (dirold == "l" and dir == "d") then
				-- this is 2junc with 0 rotation
				texture = "sprite.dirt_path.2junc"
				rotation = math.pi * -0.5
			elseif (dirold == "u" and dir == "l") or (dirold == "r" and dir == "d") then
				-- this is 2junc with 90 rotation
				texture = "sprite.dirt_path.2junc"
				rotation = math.pi
			elseif (dirold == "r" and dir == "u") or (dirold == "d" and dir == "l") then
				-- this is 2junc with 180 rotation
				texture = "sprite.dirt_path.2junc"
				--rotation = math.pi
				rotation = math.pi * 0.5
			elseif (dirold == "l" and dir == "u") or (dirold == "d" and dir == "r") then
				-- this is 2junc with -90 rotation
				texture = "sprite.dirt_path.2junc"
				rotation = 0
			end
			-- make a path at curx, cury
			local path = rn.current_scene():add_entity("dirt_path")
			rn.current_scene():entity_set_local_position(path, curx, cury, depth)
			rn.entity.prefabs.sprite.set_scale(path, path_base_scale * scale)
			rn.entity.prefabs.sprite.set_rotation(path, rotation)
			print("rotation: " .. rotation)
			rn.entity.prefabs.sprite.set_texture(path, texture)

			if dir == "r" then
				curx = curx + path_base_scale * scale * 2
			elseif dir == "l" then
				curx = curx - path_base_scale * scale * 2
			elseif dir == "u" then
				cury = cury + path_base_scale * scale * 2
			elseif dir == "d" then
				cury = cury - path_base_scale * scale * 2
			else
				tz.assert(false, "Unexpected path char `" .. dir .. "`. Must be either `r`, `l`, `u` or `d`.")
			end

			dirold = dir
		end
	end
}