for schoolname, schooldata in pairs(rn.spell.schools) do
	rn.mods.basegame.spells["lesser_" .. schoolname .. "bolt"] =
	{
		cast_duration = 1.0,
		magic_type = schoolname,
		two_handed = false,
		slot = "green",
		cast_type = "directed",
		description = "Launches a small bolt of " .. schoolname .. " in the target direction. Enemies hit suffer 5 " .. schoolname .. " damage.",
		icon = "icon." .. schoolname .. "bolt",
		static_init = function()
			if schoolname ~= "physical" then
				rn.renderer():add_texture("icon." .. schoolname .. "bolt", "basegame/res/icons/" .. schoolname .. "bolt.png")
			end
		end,
		finish = function(uuid, casterx, castery)
			local magic_type = rn.spell.spells["lesser_" .. schoolname .. "bolt"].magic_type
			local sc = rn.current_scene()

			local projectile = sc:add_entity(magic_type .. "bolt")
			rn.util.entity_set_despawn_timer(projectile, 5.0)

			rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
			sc:entity_write(projectile, "owner", uuid)
			rn.entity.prefabs.faction.copy_faction(uuid, projectile)

			local dx, dy = rn.util.entity_direction_to_target(uuid, nil, math.random(), math.random(), casterx, castery)

			rn.entity.prefabs.magic_ball_base.set_target(projectile, casterx + (dx * 999), castery + (dy * 999))
			rn.entity.prefabs.magic_ball_base.set_damage(projectile, 5)
			return projectile
		end
	}
end