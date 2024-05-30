function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end

rn.mods.basegame.spells.cruel_lie =
{
	cast_duration = 2.0,
	magic_type = "shadow",
	two_handed = true,
	slot = "blue",
	cast_type = "directed",
	description = "Imbue yourself with black magic and whisper a cruel lie, producing a shadowy ally.",
	finish = function(uuid, casterx, castery)
		local ent = rn.current_scene():add_entity("melee_monster")
		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		local dx, dy = rn.entity.prefabs.bipedal.get_face_direction(uuid)
		local len = math.sqrt(dx^2 + dy^2)
		rn.entity.prefabs.sprite.set_position(ent, x - dx * 2 / len, y - dy * 2 / len)
		rn.entity.prefabs.bipedal.set_drop_items_on_death(ent, false)
		local tex = rn.entity.prefabs.sprite.get_texture(uuid)
		if tex ~= nil then
			rn.entity.prefabs.sprite.set_texture(ent, tex)
		end
		rn.item.copy_equipment(uuid, ent)
		rn.entity.prefabs.faction.copy_faction(uuid, ent)

		local col = table.shallow_copy(rn.spell.schools.shadow.colour)
		-- shadow colour by itself is really dark, so we brighten it up a tad
		local scale_factor = 1.5
		col[1] = col[1] * scale_factor
		col[2] = col[2] * scale_factor
		col[3] = col[3] * scale_factor

		local r, g, b = rn.entity.prefabs.bipedal.get_colour(uuid)
		r = r * col[1]
		g = g * col[2]
		b = b * col[3]
		rn.entity.prefabs.bipedal.set_colour(ent, r, g, b)

		r, g, b = rn.entity.prefabs.bipedal.get_chest_colour(uuid)
		r = r * col[1]
		g = g * col[2]
		b = b * col[3]
		rn.entity.prefabs.bipedal.set_chest_colour(ent, r, g, b)

		r, g, b = rn.entity.prefabs.bipedal.get_helm_colour(uuid)
		r = r * col[1]
		g = g * col[2]
		b = b * col[3]
		rn.entity.prefabs.bipedal.set_helm_colour(ent, r, g, b)

		r, g, b = rn.entity.prefabs.bipedal.get_legs_colour(uuid)
		r = r * col[1]
		g = g * col[2]
		b = b * col[3]
		rn.entity.prefabs.bipedal.set_legs_colour(ent, r, g, b)
	end
}