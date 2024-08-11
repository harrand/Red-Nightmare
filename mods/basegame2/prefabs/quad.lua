rn.mods.basegame2.prefabs.quad =
{
	static_init = function()
		rn.renderer():add_model("quad", "basegame/res/models/plane.glb")
	end,
	pre_instantiate = function(uuid)
		return "quad"
	end,
	set_visible = function(uuid, visible)
		rn.current_scene():entity_set_subobject_visible(uuid, 2, visible)
	end,
	set_texture = function(uuid, texname)
		rn.current_scene():entity_set_subobject_texture(uuid, 2, texname)
	end,
	get_texture = function(uuid)
		return rn.current_scene():entity_get_subobject_texture(uuid, 2)
	end,
	set_normal_map = function(uuid, texname)
		rn.current_scene():entity_set_subobject_texture(uuid, 2, texname, 1)
	end,
	set_emissive_map = function(uuid, texname)
		rn.current_scene():entity_set_subobject_texture(uuid, 2, texname, 2)
	end,
	set_emissive_tint = function(uuid, r, g, b)
		rn.current_scene():entity_set_subobject_colour(uuid, 2, r, g, b, 2)
	end,
	--get_normal_map = function(uuid)
	--	return rn.current_scene():entity_get_subobject_texture(uuid, 2, 1)
	--end,
	set_colour = function(uuid, r, g, b)
		rn.current_scene():entity_set_subobject_colour(uuid, 2, r, g, b)
	end,
	get_colour = function(uuid)
		return rn.current_scene():entity_get_subobject_colour(uuid, 2)
	end,
	set_position = function(uuid, x, y)
		rn.current_scene():entity_set_local_position(uuid, x, y, 0.0)
	end,
	get_position = function(uuid)
		local x, y, z = rn.current_scene():entity_get_local_position(uuid)
		return x, y
	end,
	set_rotation = function(uuid, angle)
		-- this angle is a rotation in the z-axis as we're a 2d sprite.
		rn.current_scene():entity_set_local_rotation(uuid, 0.0, 0.0, math.sin(angle / 2.0), math.cos(angle / 2.0))
	end,
	get_rotation = function(uuid)
		local x, y, z, w = rn.current_scene():entity_get_local_rotation(uuid)
		return 2.0 * math.asin(z)
	end,
	set_scale = function(uuid, scale)
		rn.current_scene():entity_set_local_scale(uuid, scale, scale, scale)
	end,
	get_scale = function(uuid)
		local x, y, z = rn.current_scene():entity_get_local_scale(uuid)
		-- ideally they're all the same, so lets just get x
		return x
	end,

}