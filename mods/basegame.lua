local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		morbius =
		{
			pre_instantiate = function(uuid)
				return "plane"
			end,
			instantiate = function(uuid)

			end,
			update = function(uuid, delta_seconds)
				local t = rn.current_scene():entity_read(uuid, "timer") or 0.0
				t = t + delta_seconds

				local x, y, z = rn.current_scene():entity_get_local_position(uuid)
				if rn.input.keyboard():is_key_down("a") then
					x = x - 5.0 * delta_seconds
				end
				if rn.input.keyboard():is_key_down("d") then
					x = x + 5.0 * delta_seconds
				end
				rn.current_scene():entity_set_local_position(uuid, x, y, z);
			end
		}
	},
}