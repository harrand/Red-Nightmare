local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		keyboard_controlled =
		{
			instantiate = function(uuid)
				local sc = rn.current_scene()
				sc:entity_write(uuid, "control.forward", "w")
				sc:entity_write(uuid, "control.left", "a")
				sc:entity_write(uuid, "control.right", "d")
				sc:entity_write(uuid, "control.backward", "s")
			end,
			update = function(uuid, delta_seconds)
				local sc = rn.current_scene()
				local x, y, z = sc:entity_get_local_position(uuid)
				local inp = rn.input()

				local control_forward = sc:entity_read(uuid, "control.forward")
				local control_left = sc:entity_read(uuid, "control.left")
				local control_right = sc:entity_read(uuid, "control.right")
				local control_backward = sc:entity_read(uuid, "control.backward")

				local movement_speed = 5.0
				local moved = false

				if inp:is_key_down(control_left) then
					x = x - movement_speed * delta_seconds
					moved = true
				end
				if inp:is_key_down(control_right) then
					x = x + movement_speed * delta_seconds
					moved = true
				end
				if inp:is_key_down(control_forward) then
					y = y + movement_speed * delta_seconds
					moved = true
				end
				if inp:is_key_down(control_backward) then
					y = y - movement_speed * delta_seconds
					moved = true
				end
				
				if moved then
					sc:entity_set_local_position(uuid, x, y, z);
				end
			end
		},
		morbius =
		{
			pre_instantiate = function(uuid)
				return "plane"
			end,
			instantiate = function(uuid)
				rn.entity.prefabs.keyboard_controlled.instantiate(uuid)
			end,
			update = function(uuid, delta_seconds)
				rn.entity.prefabs.keyboard_controlled.update(uuid, delta_seconds)
			end
		}
	},
	levels =
	{
		startscreen =
		{
			on_load = function()
				local morb1 = rn.current_scene():add_entity("morbius")
				--rn.renderer():set_clear_colour(1.0, 0.5, 1.0, 1.0)

				local w, h = tz.window():get_dimensions()
				rn.title_string = rn.renderer():add_string(w / 2 - 250.0, h * 0.8, 40, "Red Nightmare", 0.6, 0.1, 0.15)
				rn.title_string_author = rn.renderer():add_string(w / 2 + 150, h * 0.8 - 55, 12, "by Harrand", 0.9, 0.3, 0.35)
				rn.title_string_engine = rn.renderer():add_string(20, 60, 10, "Topaz" .. tz.version.string, 1.0, 1.0, 1.0)
				rn.title_string_press_to_begin = rn.renderer():add_string(w / 2 - 110.0, h * 0.4, 10, "PRESS [ENTER] TO PLAY", 1.0, 1.0, 1.0)
			end,
			--[[
			on_update = function()
				local w, h = tz.window():get_dimensions()
				rn.title_string:set_position(w / 2 - 250.0, h * 0.8)
				rn.title_string_author:set_position(w / 2 + 150, h * 0.8 - 55)
				rn.title_string_press_to_begin:set_position(w / 2 - 110.0, h * 0.4)
			end
			--]]
		}
	}
}