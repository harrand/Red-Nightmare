rn.mods.basegame.levels.devproc0 =
{
	on_load = function()
		local player = rn.current_scene():add_entity("player_melistra")
		rn.level.data_write("player", player)

		local boundx, boundy = rn.renderer():get_view_bounds()
		boundx = math.floor((boundx * 0.5) - 2)
		boundy = math.floor((boundy * 0.5) - 2)
		for x=-boundx,boundx,2 do
			for y=-boundy,boundy,2 do
				if x == -boundx or x == boundx or y == -boundy or y == boundy then
					local ent = rn.current_scene():add_entity("invisible_wall")
					rn.entity.prefabs.sprite.set_position(ent, x, y)
				else
					-- maybe generate the wall anyway?
					local randval = math.random()
					if randval <= 0.1 then
						local ent = rn.current_scene():add_entity("invisible_wall")
						rn.entity.prefabs.sprite.set_position(ent, x, y)
					end
				end
			end
		end

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, boundx, boundy)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)

		rn.current_scene():add_entity("loot_chest")
	end
}