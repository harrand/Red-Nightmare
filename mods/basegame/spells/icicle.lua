rn.mods.basegame.spells.icicle =
{
	cast_duration = 0.6,
	magic_type = "frost",
	two_handed = false,
	slot = "blue",
	cooldown = 4.0,
	cast_type = "directed",
	description = "Launches an icicle in the target direction. Enemies hit suffer 9 frost damage.",
	icon = "icon.frostbolt",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.icicle.magic_type
		local sc = rn.current_scene()

		local projectile = sc:add_entity(magic_type .. "bolt")
		rn.util.entity_set_despawn_timer(projectile, 5.0)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
		rn.current_scene():entity_set_local_scale(projectile, 0.4, 1.25, 1.0)
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.faction.copy_faction(uuid, projectile)

		local dx, dy = rn.util.entity_direction_to_target(uuid, nil, math.random(), math.random(), casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, 9)
		return projectile
	end
}