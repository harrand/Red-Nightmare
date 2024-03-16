rn.mods.basegame.levels.clans_camp =
{
	on_load = function()
		rn.renderer():set_ambient_light(0.525, 0.495, 0.495, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 0.8)

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
		local morb1 = rn.current_scene():add_entity("player_melistra")
		rn.level.data_write("player", morb1)

		rn.item.equip(morb1, "peasant_shirt")
		rn.item.equip(morb1, "peasant_pants")
		rn.item.equip(morb1, "facial_hair_full_beard")

		local cave_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(cave_portal, -10, -10)
		rn.entity.prefabs.portal.set_colour(cave_portal, 0.5, 0.0, 0.0)
		rn.entity.prefabs.portal.set_level_destination(cave_portal, "devproc0")

		local helpme_chest = rn.current_scene():add_entity("loot_chest")
		rn.entity.prefabs.sprite.set_position(helpme_chest, -5, -8)
		rn.entity.prefabs.loot_chest.add_loot(helpme_chest, "iron_mace")
	end,
}