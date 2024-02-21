rn.mods.basegame.spells.lesser_firebolt =
{
	cast_duration = 1.25,
	magic_type = "fire",
	description = "Launches a small bolt of fire in the target direction. Enemies hit suffer ${fire_power*1.0} fire damage.",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.lesser_firebolt.magic_type
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