rn.mods.basegame.spells.savage_kick =
{
	cast_duration = 0.6,
	magic_type = "physical",
	slot = "blue",
	animation_override = "Melee1H_Kick",
	description = "Kick forward with all your might, dealing 200% physical damage to all targets in your way.",
	finish = function(uuid)
		local swing = rn.current_scene():add_entity("melee_swing_area")
		rn.entity.prefabs.faction.copy_faction(uuid, swing)
		rn.entity.prefabs.melee_swing_area.set_caster(swing, uuid)
		rn.entity.prefabs.melee_swing_area.set_base_damage(swing, 2.0)
		rn.entity.prefabs.sprite.set_scale(swing, 2.0)
		rn.entity.prefabs.timed_despawn.set_duration(swing, 0.4)
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		rn.entity.prefabs.sprite.set_position(swing, x, y)
	end
}