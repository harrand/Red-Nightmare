for schoolname, schooldata in pairs(rn.spell.schools) do
	rn.mods.basegame.spells["lesser_" .. schoolname .. "bolt"] =
	{
		cast_duration = 1.25,
		magic_type = schoolname,
		two_handed = false,
		cast_type = "directed",
		description = "Launches a small bolt of " .. schoolname .. " in the target direction. Enemies hit suffer ${" .. schoolname .. "_power*1.0} " .. schoolname .. " damage.",
		finish = function(uuid, casterx, castery)
			local magic_type = rn.spell.spells["lesser_" .. schoolname .. "bolt"].magic_type
			local sc = rn.current_scene()

			local player_uuid = rn.level.data_read("player")

			local tarx = nil
			local tary = nil
			if player_uuid == uuid then
				tarx, tary = sc:get_mouse_position()
			else
				local target = rn.entity.prefabs.base_ai.get_target(uuid)
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
			sc:entity_write(projectile, "owner", uuid)
			local dx = tarx - casterx
			local dy = tary - castery
			rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
			local power = rn.entity.prefabs.combat_stats["get_" .. magic_type .. "_power"](uuid)
			rn.entity.prefabs.magic_ball_base.set_damage(projectile, power)
			return projectile
		end
	}
end