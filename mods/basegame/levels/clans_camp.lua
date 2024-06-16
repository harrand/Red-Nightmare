rn.mods.basegame.levels.clans_camp =
{
	load_npcs = function()
		local bloke = rn.current_scene():add_entity("bipedal_human")
		rn.item.equip(bloke, "facial_hair_walrus")
		rn.item.equip(bloke, "iron_chainmail")
		rn.item.equip(bloke, "iron_platelegs")
		rn.item.equip(bloke, "steel_sword")
		rn.item.equip(bloke, "basic_torch")
		rn.current_scene():entity_play_animation(bloke, "TorchIdle", true, 1.0)
		rn.entity.prefabs.sprite.set_position(bloke, -10, -5)

		local skele = rn.current_scene():add_entity("bipedal_skeleton")
		rn.current_scene():entity_play_animation(skele, "Idle", true, 1.0)
		rn.entity.prefabs.sprite.set_position(skele, -10, 5)

		local maker = rn.current_scene():add_entity("bipedal_maker")
		rn.current_scene():entity_play_animation(maker, "Idle", true, 1.0)
		rn.entity.prefabs.sprite.set_position(maker, -13, 5)
	end,
	on_load = function()
		rn.renderer():set_ambient_light(0.525, 0.495, 0.495, 1.0)
		rn.renderer():directional_light_set_power(0.4)
		rn.renderer():directional_light_set_colour(0.8, 0.8, 0.8)
		rn.util.set_raining(0.4)

		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
		local morb1 = rn.current_scene():add_entity("player")
		rn.level.data_write("player", morb1)

		rn.entity.prefabs.dirt_path.plot_path(1, -2.0, 0, "rrrddddddllllluuuu")
		rn.entity.prefabs.dirt_path.plot_path(1, -4.0, 0, "llllllllluuuuuuuuuuu")
		rn.entity.prefabs.dirt_path.plot_path(1, 10.0, -26, "dddddd")

		rn.mods.basegame.levels.clans_camp.load_npcs()

		rn.item.equip(morb1, "peasant_pants")
		rn.item.equip(morb1, "facial_hair_full_beard")

		-- cave entrance torches
		rn.entity.prefabs.weapon_model_torch.spawn_on_ground(-44, 36)
		rn.entity.prefabs.weapon_model_torch.spawn_on_ground(-36, 36)
		local cave_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(cave_portal, -40, 40)
		rn.entity.prefabs.portal.set_colour(cave_portal, 0.6, 0.1, 0.1)
		rn.entity.prefabs.portal.set_level_destination(cave_portal, "devproc0")

		local allergiax = 10
		local allergiay = -45
		local allergia_portal = rn.current_scene():add_entity("portal")
		rn.entity.prefabs.sprite.set_position(allergia_portal, allergiax, allergiay)
		rn.entity.prefabs.portal.set_colour(allergia_portal, 0.2, 1.0, 0.4)
		rn.entity.prefabs.portal.set_level_destination(allergia_portal, "gardens_of_allergia")

		local helpme_chest = rn.current_scene():add_entity("loot_chest")
		rn.entity.prefabs.sprite.set_position(helpme_chest, -5, -8)
		rn.entity.prefabs.loot_chest.add_loot(helpme_chest, "radiant_greatsword")
		
		local temp_rathallus = rn.current_scene():add_entity("boss_rathallus")
		rn.entity.prefabs.sprite.set_position(temp_rathallus, 0, 45)
	end,
}