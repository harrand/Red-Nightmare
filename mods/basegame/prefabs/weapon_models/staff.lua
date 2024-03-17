rn.mods.basegame.prefabs.weapon_model_staff =
{
	static_init = function()
		rn.renderer():add_model("weapon.staff", "basegame/res/models/staff.glb")
	end,
	pre_instantiate = function(uuid)
		return "weapon.staff"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_set_subobject_pixelated(uuid, 2, true)
		sc:entity_set_subobject_visible(uuid, 2, true)

		local effect = sc:add_entity("cast_buildup")
		sc:entity_write(effect, "magic_type", "fire")
		sc:entity_write(effect, "time_warp", 2.5)
		sc:entity_write(effect, "power_override", 0.85)
		rn.entity.prefabs.sprite.set_scale(effect, 0.75)
		rn.entity.prefabs.sticky.stick_to_subobject(effect, uuid, 3, false, false)
		sc:entity_write(uuid, "sparkle", effect)
	end,
	on_remove = function(uuid)
		local sparkle = rn.current_scene():entity_read(uuid, "sparkle")
		if sparkle ~= nil and rn.current_scene():contains_entity(sparkle) then
			rn.current_scene():remove_entity(sparkle)
		end
	end,
	update = function(uuid, delta_seconds)
		rn.mods.basegame.prefabs.sticky.update(uuid, delta_seconds)
	end,
	put_on_ground = function(uuid, posx, posy)
		rn.mods.basegame.prefabs.sticky.stick_to(uuid, nil)
		rn.entity.prefabs.sprite.set_position(uuid, posx, posy)
		rn.entity.prefabs.sprite.set_rotation(uuid, -math.pi * 0.25)
		rn.entity.prefabs.sprite.set_colour(uuid, 0.6, 0.6, 0.6)
	end,
}