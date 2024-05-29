rn.mods.basegame.spells.flash_of_light =
{
	cast_duration = 1.25,
	magic_type = "holy",
	two_handed = false,
	cast_type = "omni",
	slot = "yellow",
	description = "Heals the caster for amount equal to ${holy_power*2.0} + 10.",
	finish = function(uuid, casterx, castery)
		local magic_type = rn.spell.spells.flash_of_light.magic_type
		local sc = rn.current_scene()
		local holy_power = rn.entity.prefabs.combat_stats.get_holy_power(uuid)
		rn.entity.prefabs.combat_stats.heal(uuid, holy_power + 10, magic_type, uuid)

		-- display a little healing effect on the caster.
		local effect = sc:add_entity("cast_buildup")
		sc:entity_write(effect, "magic_type", magic_type)
		local yoffset = 1.25
		local effect_duration = 0.75
		rn.entity.prefabs.sticky.stick_to(effect, uuid, 0.0, yoffset, 0.0)
		rn.entity.prefabs.timed_despawn.set_duration(effect, effect_duration)
	end
}