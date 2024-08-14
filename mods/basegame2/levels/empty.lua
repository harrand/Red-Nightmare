rn.mods.basegame2.levels.empty =
{
	on_load = function()
		local p = rn.current_scene():add_entity("player2")
		rn.entity.prefabs.model_humanoid.set_texture(p, "skin.man")
		rn.level.data_write("player", p)

		rn.item.drop_at(-10, 0, "basic_torch", "iron_axe")
		rn.util.set_raining(0.4)

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, -1.0, 0.0)
		rn.current_scene():entity_set_local_rotation(bg, math.sin(1.5701 * 0.5), 0, 0, math.cos(1.5701 * 0.5))
	end
}