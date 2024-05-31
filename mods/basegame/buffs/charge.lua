rn.mods.basegame.buffs.charge =
{
	name = "Charging at a target location...",
	duration = 1.0,
	on_apply = function(uuid)
		tz.report("charge begin on entity " .. uuid)
	end,
	on_remove = function(uuid)
		tz.report("charge end on entity " .. uuid)
	end,
	on_collision = function(uuid_a, uuid_b)
		rn.buff.remove(uuid_a, "charge")
	end
}