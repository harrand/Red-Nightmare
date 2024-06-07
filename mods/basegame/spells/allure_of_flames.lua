rn.mods.basegame.spells.allure_of_flames =
{
	cast_duration = 2.75,
	magic_type = "fire",
	two_handed = true,
	slot = "yellow",
	cast_type = "directed",
	cooldown = 30.0,
	description = "Launches a giant firebolt in the target direction. When the firebolt expires or hits something, you will be teleported to the location.",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.allure_of_flames.magic_type
		local sc = rn.current_scene()
		local projectile = sc:add_entity("allure_of_flame")
		rn.entity.prefabs.timed_despawn.set_duration(projectile, 7.5)
		rn.entity.prefabs.faction.copy_faction(uuid, projectile)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
		rn.entity.prefabs.sprite.set_scale(projectile, 2.0, 2.0)
		sc:entity_write(projectile, "owner", uuid)

		local dx, dy = rn.util.entity_direction_to_target(uuid, nil, math.random(), math.random(), casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
		local power = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_power"](uuid)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, power)

		local old_player = rn.level.data_read("player")
		rn.level.data_write("allure_player", old_player)
		rn.level.data_write("player", projectile)

		-- teleport the old player really far away for now...
		rn.entity.prefabs.sprite.set_position(old_player, -9999, -9999)

		return projectile
	end
}
