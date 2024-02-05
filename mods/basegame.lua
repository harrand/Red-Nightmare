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
				if rn.input():is_key_down("a") then
					x = x - 5.0 * delta_seconds
				end
				if rn.input():is_key_down("d") then
					x = x + 5.0 * delta_seconds
				end

				if rn.input():is_mouse_down("right") then
					x = 0; y = 0; z = 0
				end
				rn.current_scene():entity_set_local_position(uuid, x, y, z);
			end
		}
	},
	levels =
	{
		startscreen =
		{
			on_load = function()
				local morb1 = rn.current_scene():add_entity("morbius")
				rn.renderer():set_clear_colour(1.0, 0.5, 1.0, 1.0)
			end
		}
	}
}