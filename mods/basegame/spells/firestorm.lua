rn.mods.basegame.spells.firestorm =
{
	cast_duration = 1.25,
	magic_type = "fire",
	two_handed = true,
	cast_type = "omni",
	slot = "red",
	icon = "icon.firebolt",
	cooldown = 30.0,
	description = "Unleash a torrent of fiery death, shooting a torrent of firebolts in all directions around you, dealing 15 fire damage each.",
	finish = function(uuid, casterx, castery)
		local sc = rn.current_scene()
		local max_fireball_count = 18
		local angle_per_fireball = (math.pi * 2.0) / max_fireball_count
		for i=0,max_fireball_count-1,1 do
			local projectile = sc:add_entity("firebolt")
			rn.entity.prefabs.timed_despawn.set_duration(projectile, 5.0)
			rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
			sc:entity_write(projectile, "owner", uuid)
			rn.entity.prefabs.faction.copy_faction(uuid, projectile)
			local vecx = math.sin(angle_per_fireball * i) * 999
			local vecy = math.cos(angle_per_fireball * i) * 999
			rn.entity.prefabs.magic_ball_base.set_target(projectile, casterx + vecx, castery + vecy)

			rn.entity.prefabs.magic_ball_base.set_damage(projectile, 15)
		end
	end
}