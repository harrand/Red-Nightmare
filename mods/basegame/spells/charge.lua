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

		local dx, dy = rn.util.entity_direction_to_target(uuid, nil, math.random(), math.random())
		sc:entity_write(uuid, "charge_dirx", dx)
		sc:entity_write(uuid, "charge_diry", dy)
		rn.buff.apply(uuid, "charge_rush")
	end
}