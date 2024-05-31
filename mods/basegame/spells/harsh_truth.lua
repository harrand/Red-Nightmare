rn.mods.basegame.spells.harsh_truth =
{
	cast_duration = 2.0,
	magic_type = "shadow",
	two_handed = true,
	slot = "blue",
	cast_type = "directed",
	description = "Imbue yourself with holy magic and dictate a harsh truth, producing a radiant ally.",
	finish = function(uuid, casterx, castery)
		local ent = rn.current_scene():add_entity("holy_elemental")
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		local dx, dy = rn.entity.prefabs.bipedal.get_face_direction(uuid)
		local len = math.sqrt(dx^2 + dy^2)
		rn.entity.prefabs.sprite.set_position(ent, x - dx * 2 / len, y - dy * 2 / len)
		rn.entity.prefabs.bipedal.set_drop_items_on_death(ent, false)
		rn.entity.prefabs.faction.copy_faction(uuid, ent)
	end
}
