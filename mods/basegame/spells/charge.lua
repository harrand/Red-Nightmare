rn.mods.basegame.spells.charge =
{
	cast_duration = 0.0,
	magic_type = "physical",
	slot = "yellow",
	cooldown = 12.0,
	description = "Charge at the target direction at high speed, crashing into anything that blocks your path for high physical damage.",
	finish = function(uuid, casterx, castery)
		if rn.buff.is_applied(uuid, "charge_rush") then return end
		local player_uuid = rn.level.data_read("player")
		local sc = rn.current_scene()

		local tarx = nil
		local tary = nil
		if player_uuid == uuid then
			tarx, tary = sc:get_mouse_position()
		else
			local target = rn.util.entity_get_target(uuid)
			if target ~= nil and sc:contains_entity(target) then
				tarx, tary = rn.entity.prefabs.sprite.get_position(target)
			else
				-- no target and nothing to shoot at... charge randomly?
				tarx = math.random()
				tary = math.random()
			end
		end

		if tarx == nil or tary == nil then
			return
		end

		local px, py = rn.entity.prefabs.sprite.get_position(uuid)
		local dx = tarx - px
		local dy = tary - py
		sc:entity_write(uuid, "charge_dirx", dx)
		sc:entity_write(uuid, "charge_diry", dy)
		rn.buff.apply(uuid, "charge_rush")
	end
}