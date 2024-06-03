rn.mods.basegame.spells.summon_zombie =
{
	cast_duration = 1.25,
	magic_type = "shadow",
	two_handed = false,
	cast_type = "omni",
	cooldown = 16.0,
	slot = "blue",
	description = "Summons a zombie (at the origin of the world)",
	finish = function(uuid, casterx, castery)
		local ent = rn.current_scene():add_entity("zombie")
		rn.entity.prefabs.bipedal.set_drop_items_on_death(ent, false)
		rn.entity.prefabs.faction.copy_faction(uuid, ent)
		rn.entity.prefabs.sprite.set_position(ent, rn.entity.prefabs.sprite.get_position(uuid))
		rn.item.copy_equipment(uuid, ent)
	end
}
