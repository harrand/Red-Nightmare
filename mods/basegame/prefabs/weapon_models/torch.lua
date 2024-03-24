rn.mods.basegame.prefabs.weapon_model_torch =
{
	static_init = function()
		rn.renderer():add_model("weapon.torch", "basegame/res/models/torch.glb")
	end,
	pre_instantiate = function(uuid)
		return "weapon.torch"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_set_subobject_pixelated(uuid, 2, true)
		sc:entity_set_subobject_visible(uuid, 2, true)

		local effect = sc:add_entity("cast_buildup")
		sc:entity_write(effect, "magic_type", "fire")
		sc:entity_write(effect, "time_warp", 2.5)
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
		local effect = rn.current_scene():entity_read(uuid, "sparkle")
		if effect ~= nil and rn.current_scene():contains_entity(effect) then
			-- torch flicker.
			local timer = rn.current_scene():entity_read(uuid, "timer") or 0.0
			timer = timer + delta_seconds
			local torch_min_power = 0.8
			rn.current_scene():entity_write(effect, "power_override", math.abs(math.sin(timer * 15.0) * 0.1) + torch_min_power + (math.cos(1.0 / delta_seconds) * torch_min_power * 0.04))
			rn.current_scene():entity_write(uuid, "timer", timer)
		end
	end,
	put_on_ground = function(uuid, posx, posy)
		rn.mods.basegame.prefabs.sticky.stick_to(uuid, nil)
		rn.entity.prefabs.sprite.set_position(uuid, posx, posy)
		rn.entity.prefabs.sprite.set_rotation(uuid, -math.pi * 0.25)
		rn.entity.prefabs.sprite.set_colour(uuid, 0.6, 0.6, 0.6)
	end,
	spawn_on_ground = function(posx, posy)
		local torch = rn.current_scene():add_entity("weapon_model_torch")
		rn.entity.prefabs.weapon_model_torch.put_on_ground(torch, posx, posy)
	end
}