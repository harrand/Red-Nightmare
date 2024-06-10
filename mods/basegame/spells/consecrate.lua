rn.mods.basegame.spells.consecrate =
{
	cast_duration = 1.75,
	magic_type = "holy",
	two_handed = true,
	cast_type = "omni",
	slot = "red",
	icon = "icon.holybolt",
	cooldown = 20.0,
	description = "Consecrate the ground beneath you, granting a blessing to allies who stand within it, but inflicting holy damage on enemies.",
	finish = function(uuid, casterx, castery)
		local consx, consy = rn.entity.prefabs.sprite.get_position(uuid)
		local cons = rn.current_scene():add_entity("consecration_effect")
		rn.entity.prefabs.sprite.set_position(cons, consx, consy)
		rn.current_scene():entity_write(cons, "magic_type", rn.mods.basegame.spells.consecrate.magic_type)
		rn.entity.prefabs.faction.copy_faction(uuid, cons)
		rn.entity.prefabs.consecration_effect.set_caster(cons, uuid)
		rn.entity.prefabs.consecration_effect.set_base_damage(cons, 1.0)
		rn.entity.prefabs.sprite.set_scale(cons, 5.0)
		rn.util.entity_set_despawn_timer(cons, 12.0)
	end
}