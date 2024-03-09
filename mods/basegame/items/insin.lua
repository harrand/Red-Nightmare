rn.mods.basegame.items.insin =
{
	tooltip = [[
		+3 Physical Power
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, 3.0)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -3.0)
	end,
	on_cast = function(uuid, spellname, casterx, castery)
		if spellname ~= "melee" then return end
		local dx, dy = rn.entity.prefabs.bipedal.get_face_direction(uuid)
		local angle = math.atan(dy, dx)
		-- we want to cast 3 fireballs
		-- one exactly at dx, dy
		-- one 30 degrees less than whatever that vector represents.
		-- and one more 30 degrees more.
		local dxbig = -math.cos(angle + (math.pi / 6.0))
		local dybig = -math.sin(angle + (math.pi / 6.0))
		local dxsmall = -math.cos(angle - (math.pi / 6.0))
		local dysmall = -math.sin(angle - (math.pi / 6.0))
		dx = -dx
		dy = -dy

		local projmid = rn.spell.spells.lesser_firebolt.finish(uuid, casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projmid, dx * 999, dy * 999)
		local projbig = rn.spell.spells.lesser_firebolt.finish(uuid, casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projbig, dxbig * 999, dybig * 999)
		local projsmall = rn.spell.spells.lesser_firebolt.finish(uuid, casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_target(projsmall, dxsmall * 999, dysmall * 999)
	end,
	slot = rn.item.slot.right_hand,
	colour = rn.spell.schools.fire.colour,
	rarity = "legendary",
	weapon_prefab = "weapon_morning_star"
}