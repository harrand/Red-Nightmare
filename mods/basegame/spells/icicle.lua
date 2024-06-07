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

		local player_uuid = rn.level.data_read("player")

		local tarx = nil
		local tary = nil
		if player_uuid == uuid then
			tarx, tary = sc:get_mouse_position()
		else
			local target = rn.util.entity_get_target(uuid)
			if target ~= nil and sc:contains_entity(target) then
				tarx, tary = rn.entity.prefabs.sprite.get_position(target)
			else
				-- no target and nothing to shoot at... shoot randomly?
				tarx = math.random()
				tary = math.random()
			end
		end

		if tarx == nil or tary == nil then
			return
		end

		local projectile = sc:add_entity(magic_type .. "bolt")
		rn.entity.prefabs.timed_despawn.set_duration(projectile, 5.0)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
		rn.current_scene():entity_set_local_scale(projectile, 0.4, 1.25, 1.0)
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.faction.copy_faction(uuid, projectile)
		local dx = tarx - casterx
		local dy = tary - castery
		rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, 9)
		return projectile
	end
}