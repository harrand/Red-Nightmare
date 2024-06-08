rn.mods.basegame.spells.spellsteal =
{
	cast_duration = 0.9,
	magic_type = "shadow",
	two_handed = true,
	slot = "red",
	cast_type = "directed",
	description = "Hurl a whispering current of void, attaching itself to the first enemy it touches. The next spell it casts will then be duplicated, replacing Spellsteal for a single time.",
	icon = "icon.shadowbolt",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.icicle.magic_type
		local sc = rn.current_scene()

		local projectile = sc:add_entity("spellsteal_projectile")
		rn.util.entity_set_despawn_timer(projectile, 5.0)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
		rn.current_scene():entity_set_local_scale(projectile, 0.4, 1.25, 1.0)
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.faction.copy_faction(uuid, projectile)
		local dx, dy = rn.util.entity_direction_to_target(uuid, nil, math.random(), math.random(), casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
		return projectile
	end
}