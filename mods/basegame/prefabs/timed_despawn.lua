rn.mods.basegame.prefabs.timed_despawn = 
{
	-- set the internal variable "duration" to a number of seconds.
	-- when it hits 0 the entity will automatically despawn.
	set_duration = function(uuid, dur)
		rn.current_scene():entity_write(uuid, "duration", dur)
	end,
	update = function(uuid, delta_seconds)
		local t = rn.current_scene():entity_read(uuid, "duration") or 999999999
		t = t - delta_seconds
		if t <= 0.0 then
			rn.current_scene():remove_entity(uuid)
		else
			rn.current_scene():entity_write(uuid, "duration", t)
		end
	end
}