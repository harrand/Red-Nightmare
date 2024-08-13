rn.mods.basegame.spells.melee =
{
	static_init = function()
		rn.renderer():add_texture("icon.melee", "basegame/res/icons/melee.png")
	end,
	cast_duration = 0.6,
	magic_type = "physical",
	slot = "green",
	icon = "icon.melee",
	cooldown = 1.0,
	description = "Attack with your weapon, dealing 100% physical damage.",
	finish = function(uuid)
		local swing = rn.current_scene():add_entity("melee_swing_area")
		rn.entity.prefabs.faction.copy_faction(uuid, swing)
		rn.entity.prefabs.melee_swing_area.set_caster(swing, uuid)
		rn.entity.prefabs.melee_swing_area.set_base_damage(swing, 1.0)
		rn.entity.prefabs.sprite.set_scale(swing, 2.0)
		rn.util.entity_set_despawn_timer(swing, 0.4)
		local x, y
		if rn.current_scene():entity_get_model(uuid) == "bipedal" or rn.current_scene():entity_get_model(uuid) == "humanoid" then
			x, y, _ = rn.current_scene():entity_get_global_position(uuid, rn.entity.prefabs.bipedal.right_hand)
		else
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		end
		rn.entity.prefabs.sprite.set_position(swing, x, y)
	end
}