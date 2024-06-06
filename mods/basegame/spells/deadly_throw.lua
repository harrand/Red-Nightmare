rn.mods.basegame.spells.deadly_throw =
{
	cast_duration = 0.6,
	magic_type = "physical",
	slot = "red",
	icon = "icon.melee",
	animation_override = "Cast1H_Directed",
	cooldown = 2.0,
	description = "Throws your weapon in the target location, dealing massive physical damage.",
	finish = function(uuid, casterx, castery)
		local slot = rn.item.slot.right_hand
		local sc = rn.current_scene();
		local weapon_entity = sc:entity_read(uuid, "weapon_entity" .. slot)
		sc:entity_write(uuid, "weapon_entity" .. slot, nil)
		sc:entity_write(uuid, "thrown_weapon", weapon_entity)
		local itemname = rn.item.get_equipped(uuid, slot)
		sc:entity_write(uuid, "thrown_weapon_name", itemname)
		sc:entity_write(uuid, "equipment." .. tostring(slot), nil)
		local itemdata = rn.item.items[itemname]

		local dmg = rn.entity.prefabs.combat_stats.get_physical_damage(uuid, 10)
		if itemdata.on_unequip ~= nil then
			itemdata.on_unequip(uuid)
		end

		local projectile = rn.spell.spells.lesser_physicalbolt.finish(uuid, casterx, castery)
		rn.entity.prefabs.magic_ball_base.set_damage(projectile, dmg)
		-- pretend as if the projectile was equipping the weapon so it benefits from its stats.
		if itemdata.on_equip ~= nil then
			itemdata.on_equip(projectile)
		end
		rn.entity.prefabs.sticky.stick_to(weapon_entity, projectile, 0, 0, 0)
	end,
	on_cast_begin = function(uuid)
		local slot = rn.item.slot.right_hand
		local weapon_entity = rn.current_scene():entity_read(uuid, "weapon_entity" .. slot)
		if weapon_entity == nil then
			-- no obvious weapon equipped. cancel cast.
			rn.spell.clear(uuid)
			return
		end
	end
}