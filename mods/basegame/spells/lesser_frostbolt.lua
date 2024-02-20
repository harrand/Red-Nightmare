rn.mods.basegame.spells.lesser_frostbolt =
{
	cast_duration = 2.5,
	magic_type = "frost",
	description = "Launches a small shard of frost in the target direction. Enemies hit suffer ${frost_power*1.0} frost damage.",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.lesser_frostbolt.magic_type
		local sc = rn.current_scene()
		local projectile = sc:add_entity(magic_type .. "bolt")
		rn.entity.prefabs.timed_despawn.set_duration(projectile, 5.0)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)

		local mx, my = sc:get_mouse_position()
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, mx, my)
		local power = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_power"](uuid)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, power)
	end
}