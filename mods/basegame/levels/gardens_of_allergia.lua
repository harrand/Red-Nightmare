rn.mods.basegame.levels.gardens_of_allergia =
{
	on_load = function()
		rn.renderer():set_ambient_light(0.425, 0.595, 0.495, 1.0)
		rn.renderer():directional_light_set_power(0.6)
		rn.renderer():directional_light_set_colour(0.6, 1.0, 0.6)
		rn.renderer():set_precipitation(0.1, 1.0, 0.1, 0.3, -0.05, 0.3)

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
		local morb1 = rn.current_scene():add_entity("player")
		rn.level.data_write("player", morb1)

		rn.item.equip(morb1, "peasant_pants")
		rn.item.equip(morb1, "facial_hair_full_beard")

		-- clans camp
		local campx = 0
		local campy = 5
		rn.entity.prefabs.weapon_model_torch.spawn_on_ground(campx - 4, campy - 4)
		rn.entity.prefabs.weapon_model_torch.spawn_on_ground(campx + 4, campy - 4)
		local camp_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(camp_portal, campx, campy)
		rn.entity.prefabs.portal.set_colour(camp_portal, 0.3, 0.3, 1.0)
		rn.entity.prefabs.portal.set_level_destination(camp_portal, "clans_camp")
	end,
}