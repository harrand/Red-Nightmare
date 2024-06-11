rn.mods.basegame.prefabs.weapon_model_lightbringer =
{
	pre_instantiate = function(uuid)
		return "weapon.sword2h"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_set_subobject_pixelated(uuid, 2, true)
		sc:entity_set_subobject_visible(uuid, 2, true)

		local effect_bottom = sc:add_entity("cast_buildup")
		sc:entity_write(effect_bottom, "magic_type", "holy")
		sc:entity_write(effect_bottom, "time_warp", 2.5)
		rn.entity.prefabs.sprite.set_scale(effect_bottom, 0.75)
		rn.entity.prefabs.sticky.stick_to_subobject(effect_bottom, uuid, 3, false, false)
		sc:entity_write(uuid, "sparkle_bottom", effect_bottom)

		local effect_top = sc:add_entity("cast_buildup")
		sc:entity_write(effect_top, "magic_type", "holy")
		sc:entity_write(effect_top, "time_warp", 2.5)
		rn.entity.prefabs.sprite.set_scale(effect_top, 0.75)
		rn.entity.prefabs.sticky.stick_to_subobject(effect_top, uuid, 4, false, false)
		sc:entity_write(uuid, "sparkle_top", effect_top)
	end,
	on_remove = function(uuid)
		local sparkle_bottom = rn.current_scene():entity_read(uuid, "sparkle_bottom")
		if sparkle_bottom ~= nil and rn.current_scene():contains_entity(sparkle_bottom) then
			rn.current_scene():remove_entity(sparkle_bottom)
		end

		local sparkle_top = rn.current_scene():entity_read(uuid, "sparkle_top")
		if sparkle_top ~= nil and rn.current_scene():contains_entity(sparkle_top) then
			rn.current_scene():remove_entity(sparkle_top)
		end
	end,
	update = function(uuid, delta_seconds)
		rn.mods.basegame.prefabs.sticky.update(uuid, delta_seconds)
		local effect_bottom = rn.current_scene():entity_read(uuid, "sparkle_bottom")
		local timer = rn.current_scene():entity_read(uuid, "timer") or 0.0
		timer = timer + delta_seconds
		local torch_min_power = 0.6
		if effect_bottom ~= nil and rn.current_scene():contains_entity(effect_bottom) then
			-- glow flicker.
			rn.current_scene():entity_write(effect_bottom, "power_override", math.abs(math.sin(timer * 2.0) * 0.1) + torch_min_power)
		end

		local effect_top = rn.current_scene():entity_read(uuid, "sparkle_top")
		if effect_top ~= nil and rn.current_scene():contains_entity(effect_top) then
			-- glow flicker.
			rn.current_scene():entity_write(effect_top, "power_override", math.abs(math.sin(timer * 2.0) * 0.1) + torch_min_power)
		end
		rn.current_scene():entity_write(uuid, "timer", timer)
	end,
}