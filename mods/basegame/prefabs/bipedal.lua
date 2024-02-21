local base_subobj = 3
-- note: these 3 may or may not be mixed up.
local chest_subobj = 5
local helm_subobj = 7
-- note: subobject 16 also seems to be around the head?
local legs_subobj = 9

-- 18 seems to be left shoulder. 19 elbow. 20 hand.
-- 22 right shoulder, 23 elbow. 24 hand.

local base_scale = 0.2

rn.mods.basegame.prefabs.bipedal =
{
	description = "Entity is a 3D bipedal animated humanoid",
	left_hand = 20,
	right_hand = 24,
	static_init = function()
		rn.renderer():add_model("bipedal", "basegame/res/models/bipedal.glb")
	end,
	pre_instantiate = function(uuid)
		return "bipedal"
	end,
	instantiate = function(uuid)
		rn.current_scene():entity_set_subobject_pixelated(uuid, base_subobj, true)
		rn.current_scene():entity_set_subobject_pixelated(uuid, helm_subobj, true)
		rn.current_scene():entity_set_subobject_pixelated(uuid, chest_subobj, true)
		rn.current_scene():entity_set_subobject_pixelated(uuid, legs_subobj, true)
		rn.entity.prefabs.bipedal.set_scale(uuid, 1.0, 1.0, 1.0)
		rn.entity.prefabs.bipedal.set_visible(uuid, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, helm_subobj, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, chest_subobj, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, legs_subobj, true)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local currently_playing = sc:entity_get_playing_animation(uuid)
		local moving = sc:entity_read(uuid, "moving")
		local should_stop_move_animation = currently_playing == "Run" and not moving
		if currently_playing == nil or should_stop_move_animation then
			sc:entity_play_animation(uuid, "CastIdle")
		end
		sc:entity_write(uuid, "moving", false)
	end,
	on_move = function(uuid, xdiff, ydiff, zdiff)
		local sc = rn.current_scene()
		local currently_playing = sc:entity_get_playing_animation(uuid)
		if currently_playing ~= "Run" then
			rn.current_scene():entity_play_animation(uuid, "Run")
		end
		-- if we're moving both horizontally and vertically, always prefer horizontal facing.
		if math.abs(xdiff) >= math.abs(ydiff) then
			ydiff = 0
		end
		rn.entity.prefabs.bipedal.face_direction(uuid, -xdiff, -ydiff)
		sc:entity_write(uuid, "moving", true)
	end,
	on_cast_begin = function(uuid, spellname)
		local spelldata = rn.spell.spells[spellname]
		local cast_duration = spelldata.cast_duration
		local animation_duration = rn.current_scene():entity_get_animation_length(uuid, "Cast1H_Directed")
		-- note: the end frame of the animation is unlikely to be when we want the cast to go off.
		-- for now, let's say we want the anim to be 65% done when the cast actually goes off.
		animation_duration = animation_duration * 0.65
		rn.entity.prefabs.bipedal.play_animation(uuid, "Cast1H_Directed", false, animation_duration / cast_duration)
	end,
	play_animation = function(uuid, animation_name, loop, time_warp)
		rn.current_scene():entity_play_animation(uuid, animation_name, loop, time_warp)
	end,
	set_subobject_visible = function(uuid, subobject, visible)
		rn.current_scene():entity_set_subobject_visible(uuid, subobject, visible)
	end,
	set_visible = function(uuid, visible)
		rn.entity.prefabs.bipedal.set_subobject_visible(uuid, 3, visible)	
	end,
	set_texture = function(uuid, texname)
		rn.current_scene():entity_set_subobject_texture(uuid, 3, texname)
	end,
	get_texture = function(uuid)
		return rn.current_scene():entity_get_subobject_texture(uuid, 3)
	end,
	set_colour = function(uuid, r, g, b)
		rn.current_scene():entity_set_subobject_colour(uuid, 3, r, g, b)
	end,
	get_colour = function(uuid)
		return rn.current_scene():entity_get_subobject_colour(uuid, 3)
	end,
	set_position = function(uuid, x, y)
		rn.entity.prefabs.sprite.set_position(uuid, x, y)
	end,
	get_position = function(uuid)
		return rn.entity.prefabs.sprite.get_position(uuid)
	end,
	set_rotation = function(uuid, ry, rx, rz)
		-- euler to quaternion
		local sin = math.sin
		local cos = math.cos
		local qx = sin(rz/2.0) * cos(rx/2.0) * cos(ry/2.0) - cos(rz/2.0) * sin(rx/2.0) * sin(ry/2.0)
		local qy = cos(rz/2.0) * sin(rx/2.0) * cos(ry/2.0) + sin(rz/2.0) * cos(rx/2.0) * sin(ry/2.0)
		local qz = cos(rz/2.0) * cos(rx/2.0) * sin(ry/2.0) - sin(rz/2.0) * sin(rx/2.0) * cos(ry/2.0)
		local qw = cos(rz/2.0) * cos(rx/2.0) * cos(ry/2.0) + sin(rz/2.0) * sin(rx/2.0) * sin(ry/2.0)
		rn.current_scene():entity_set_local_rotation(uuid, qx, qy, qz, qw)
	end,
	get_rotation = function(uuid)
		local x, y, z, w = rn.current_scene():entity_get_local_rotation(uuid)
		local t0 = 2.0 * (w * x + y * z)
		local t1 = 1.0 - 2.0 * (x * x + y * y)
		local roll = math.atan(t0, t1)
		local t2 = 2.0 * (w * y - z * x)
		if t2 > 1.0 then t2 = 1.0 end
		if t2 < -1.0 then t2 = -1.0 end
		local pitch = math.asin(t2)
		local t3 = 2.0 * (w * z + x * y)
		local t4 = 1.0 - 2.0 * (y * y + z * z)
		local yaw = math.atan(t3, t4)
		return yaw, pitch, roll
	end,
	face_direction = function(uuid, dx, dy)
		-- face in a directional vector. useful for calculating casting directions.
		local tilt_factor = 0.125
		if math.abs(dx) > math.abs(dy) then
			local signx = math.abs(dx)/dx
			-- if face right, rx and rz is positive
			-- if face left, rx is negative and rz is positive
			rn.entity.prefabs.bipedal.set_rotation(uuid, 0.0, -signx * math.pi / 2.0, 0.0)
		else
			local signy = math.abs(dy)/dy
			rn.entity.prefabs.bipedal.set_rotation(uuid, 0.0, math.max(-signy, 0.0) * math.pi, math.pi * signy * tilt_factor)
		end
	end,
	set_scale = function(uuid, sx, sy, sz)
		rn.current_scene():entity_set_local_scale(uuid, sx * base_scale, sy * base_scale, sz * base_scale)
	end,
	get_scale = function(uuid)
		local x, y, z = rn.current_scene():entity_get_local_scale(uuid)
		x = x * base_scale
		y = y * base_scale
		z = z * base_scale
		return x, y, z
	end
}