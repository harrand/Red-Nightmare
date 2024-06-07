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

		local projectile = sc:add_entity("spellsteal_projectile")
		rn.entity.prefabs.timed_despawn.set_duration(projectile, 5.0)

		rn.entity.prefabs.sprite.set_position(projectile, casterx, castery)
		rn.current_scene():entity_set_local_scale(projectile, 0.4, 1.25, 1.0)
		sc:entity_write(projectile, "owner", uuid)
		rn.entity.prefabs.faction.copy_faction(uuid, projectile)
		local dx = tarx - casterx
		local dy = tary - castery
		rn.entity.prefabs.magic_ball_base.set_target(projectile, dx * 999, dy * 999)
		return projectile
	end
}