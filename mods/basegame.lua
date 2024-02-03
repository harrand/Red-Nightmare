local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		morbius =
		{
			pre_instantiate = function(uuid)
				print("i'm preparing to morb...")
				return "plane"
			end,
			instantiate = function(uuid)
				print("it's morbin' time!")
			end,
			update = function(uuid, delta_seconds)
				local t = rn.current_scene():entity_read(uuid, "timer") or 0.0
				t = t + delta_seconds

				local x, y, z = rn.current_scene():entity_get_local_position(uuid)
				rn.current_scene():entity_set_local_position(uuid, math.sin(t), y, z);
				rn.current_scene():entity_write(uuid, "timer", t)
			end
		}
	},
}