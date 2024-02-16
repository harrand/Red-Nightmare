rn.mods.basegame.spells.lesser_firebolt =
{
	cast_duration = 2.5,
	finish = function(uuid)
		local sc = rn.current_scene()
		local projectile = sc:add_entity("firebolt")

		-- note: this assumes the caster is a sprite. this will not be the case for long.
		local casterx, castery = rn.entity.prefabs.sprite.get_position(uuid)
		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)

		local mx, my = sc:get_mouse_position()
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, mx, my)
		local fire_power = rn.entity.prefabs.combat_stats.get_fire_power(uuid)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, fire_power)
	end
}