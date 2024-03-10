rn.mods.basegame.spells.melee =
{
	cast_duration = 0.75,
	magic_type = "physical",
	finish = function(uuid)
		local swing = rn.current_scene():add_entity("melee_swing_area")
		rn.entity.prefabs.melee_swing_area.set_caster(swing, uuid)
		rn.entity.prefabs.sprite.set_scale(swing, 2.0)
		rn.entity.prefabs.timed_despawn.set_duration(swing, 1.0)
		local x, y
		if rn.current_scene():entity_get_model(uuid) == "bipedal" then
			x, y, _ = rn.current_scene():entity_get_global_position(uuid, rn.entity.prefabs.bipedal.right_hand)
		else
			local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		end
		rn.entity.prefabs.sprite.set_position(swing, x, y)
	end
}