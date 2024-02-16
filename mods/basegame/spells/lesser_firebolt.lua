rn.mods.basegame.spells.lesser_firebolt =
{
	cast_duration = 2.5,
	magic_type = "fire",
	finish = function(uuid)
		local magic_type = rn.spell.spells.lesser_firebolt.magic_type
		local sc = rn.current_scene()
		local projectile = sc:add_entity(magic_type .. "bolt")

		-- note: this assumes the caster is a sprite. this will not be the case for long.
		local casterx, castery = rn.entity.prefabs.sprite.get_position(uuid)
		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)

		local mx, my = sc:get_mouse_position()
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, mx, my)
		local power = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_power"](uuid)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, power)
	end
}