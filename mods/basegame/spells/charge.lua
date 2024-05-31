rn.mods.basegame.spells.charge =
{
	cast_duration = 0.0,
	magic_type = "physical",
	slot = "yellow",
	description = "Charge at the target direction at high speed, crashing into anything that blocks your path for high physical damage.",
	finish = function(uuid, casterx, castery)
		if rn.buff.is_applied(uuid, "charge_rush") then return end
		local mx, my = rn.current_scene():get_mouse_position()
		local px, py = rn.entity.prefabs.sprite.get_position(uuid)
		local dx = mx - px
		local dy = my - py
		local sc = rn.current_scene()
		sc:entity_write(uuid, "charge_dirx", dx)
		sc:entity_write(uuid, "charge_diry", dy)
		rn.buff.apply(uuid, "charge_rush")
	end
}