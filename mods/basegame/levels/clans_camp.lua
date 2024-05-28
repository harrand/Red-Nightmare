rn.mods.basegame.levels.clans_camp =
{
	load_npcs = function()
		local bloke = rn.current_scene():add_entity("bipedal")
		rn.entity.prefabs.bipedal.set_texture(bloke, "skin.man")
		rn.item.equip(bloke, "facial_hair_walrus")
		rn.item.equip(bloke, "iron_chainmail")
		rn.item.equip(bloke, "iron_platelegs")
		rn.item.equip(bloke, "steel_sword")
		rn.item.equip(bloke, "basic_torch")
		rn.current_scene():entity_play_animation(bloke, "TorchIdle", true, 1.0)
		rn.entity.prefabs.sprite.set_position(bloke, -10, -5)

		local skele = rn.current_scene():add_entity("bipedal")
		rn.entity.prefabs.bipedal.set_texture(skele, "skin.skeleton")
		rn.current_scene():entity_play_animation(skele, "Idle", true, 1.0)
		rn.entity.prefabs.sprite.set_position(skele, -10, 5)
	end,
	on_load = function()
		rn.renderer():set_ambient_light(0.525, 0.495, 0.495, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 0.8)

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
		local morb1 = rn.current_scene():add_entity("player")
		rn.level.data_write("player", morb1)

		rn.mods.basegame.levels.clans_camp.load_npcs()

		rn.item.equip(morb1, "peasant_pants")
		rn.item.equip(morb1, "facial_hair_full_beard")

		local cave_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(cave_portal, -10, -10)
		rn.entity.prefabs.portal.set_colour(cave_portal, 0.6, 0.1, 0.1)
		rn.entity.prefabs.portal.set_level_destination(cave_portal, "devproc0")

		local helpme_chest = rn.current_scene():add_entity("loot_chest")
		rn.entity.prefabs.sprite.set_position(helpme_chest, -5, -8)
		rn.entity.prefabs.loot_chest.add_loot(helpme_chest, "basic_torch")
	end,
}