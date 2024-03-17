rn.mods.basegame.items.insin =
{
	tooltip = [[
		+20 Fire Power
		+10 Physical Power
		-40% Haste
		Equip: Your melee attacks now unleash a barrage of fireballs in your attack direction.

		"In'sin vex omi'thi'i"
	]],
	on_equip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, 10.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, 20.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, -0.4)
	end,
	on_unequip = function(uuid)
		rn.entity.prefabs.combat_stats.apply_flat_increased_physical_power(uuid, -10.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_fire_power(uuid, -20.0)
		rn.entity.prefabs.combat_stats.apply_flat_increased_haste(uuid, 0.4)
	end,
	on_update = function(uuid, delta_seconds)
		if not rn.entity.prefabs.combat_stats.is_alive(uuid) then return end

		local sc = rn.current_scene()
		local fiery_minion_uuid = sc:entity_read(uuid, "insin_minion")
		local fiery_minion_summon_icd = sc:entity_read(uuid, "insin_minion_icd") or 0.0
		if fiery_minion_uuid == nil or rn.entity.prefabs.combat_stats.is_dead(fiery_minion_uuid) then
			if fiery_minion_summon_icd <= 0.0 then
				-- summon minion
				fiery_minion_uuid = sc:add_entity("fire_elemental")
				fiery_minion_summon_icd = 10.0

				sc:entity_write(fiery_minion_uuid, "owner", uuid)
				sc:entity_write(uuid, "insin_minion", fiery_minion_uuid)
			else
				-- reduce the icd.
				fiery_minion_summon_icd = fiery_minion_summon_icd - delta_seconds
			end
			sc:entity_write(uuid, "insin_minion_icd", fiery_minion_summon_icd)
		end
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
	two_handed = true,
	colour = rn.spell.schools.fire.colour,
	rarity = "legendary",
	weapon_prefab = "weapon_model_maul",
	weapon_class = "mace2h"
}