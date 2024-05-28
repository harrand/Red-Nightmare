rn.mods.basegame.spells.firestorm =
{
	cast_duration = 1.25,
	magic_type = "fire",
	two_handed = true,
	cast_type = "omni",
	description = "Unleash a torrent of fiery death, shooting a barrage of firebolts in all directions around you, dealing damage equal to ${fire_power*2.0} fire damage each.",
	--[[
	advance = function(uuid, progress)
		local magic_type = rn.mods.basegame.spells.firestorm.magic_type
		local colour = rn.spell.schools[magic_type].colour
		rn.renderer():set_ambient_light(0.2 + colour[1] * progress * 0.5, 0.05 + colour[2] * progress * 0.5, 0.05 + colour[3] * progress * 0.5)
	end,
	--]]
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

			local power = rn.entity.prefabs.combat_stats["get_fire_power"](uuid)
			rn.entity.prefabs.magic_ball_base.set_damage(projectile, power * 2.0)
		end
	end
}