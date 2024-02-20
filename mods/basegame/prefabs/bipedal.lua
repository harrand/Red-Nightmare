rn.mods.basegame.prefabs.bipedal =
{
	description = "Entity is a 3D bipedal animated humanoid",
	static_init = function()
		rn.renderer():add_model("bipedal", "basegame/res/models/bipedal.glb")
	end,
	pre_instantiate = function(uuid)
		return "bipedal"
	end,
	instantiate = function(uuid)
		rn.current_scene():entity_set_subobject_pixelated(uuid, 3, true)
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
	set_rotation = function(uuid, rx, ry, rz)
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
		return pitch, yaw, roll
	end,
	set_scale = function(uuid, sx, sy, sz)
		rn.current_scene():entity_set_local_scale(uuid, sx, sy, sz)
	end,
	get_scale = function(uuid)
		return rn.current_scene():entity_get_local_scale(uuid)
	end
}