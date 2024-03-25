rn.mods.basegame.levels.dev0 =
{
	static_init = function()
		rn.renderer():add_texture("background.grassy", "basegame/res/textures/background_grassy.png")
		rn.renderer():add_texture("background.grassy_normals", "basegame/res/textures/background_grassy_normals.png")
	end,
	on_load = function()
		local bg = rn.current_scene():add_entity("sprite")
		rn.entity.prefabs.sprite.set_scale(bg, 69, 69)
		rn.entity.prefabs.sprite.set_texture(bg, "background.grassy")
		rn.entity.prefabs.sprite.set_normal_map(bg, "background.grassy_normals")
		rn.current_scene():entity_set_local_position(bg, 0.0, 0.0, -2.0)
		local morb1 = rn.current_scene():add_entity("player")
		rn.item.equip(morb1, "iron_sallet")
		rn.item.equip(morb1, "iron_chainmail")
		rn.item.equip(morb1, "iron_chainlegs")
		rn.item.equip(morb1, "holy_water_sprinkler")
		rn.level.data_write("player", morb1)

		-- add a bunch more randoms
		math.randomseed(os.time())
		for i=0,128,1 do
			local ty = "firebolt"
			if i%2 == 0 then ty = "frostbolt" end
			local morbx = rn.current_scene():add_entity(ty)
			rn.entity.prefabs.magic_ball_base.set_damage(morbx, 2)
			local randx = math.random(-40, 40)
			local randy = math.random(-40, 40)
			rn.entity.prefabs.sprite.set_position(morbx, randx, randy)
		end

	end,
}