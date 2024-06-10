rn.mods.basegame.prefabs.boss_rathallus =
{
	description = "Rathallus Ceitus, prince of the Ancient Empire.",
	static_init = function()
		rn.renderer():add_texture("skin.npc_rathallus", "basegame/res/skins/npc_rathallus.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.melee_monster.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.melee_monster.instantiate(uuid)
		rn.entity.prefabs.bipedal.set_texture(uuid, "skin.npc_rathallus")

		rn.entity.prefabs.spell_slots.equip_spell(uuid, "melee")
		rn.entity.prefabs.spell_slots.equip_spell(uuid, "savage_kick")
		rn.entity.prefabs.spell_slots.equip_spell(uuid, "flash_of_light")
		rn.entity.prefabs.spell_slots.equip_spell(uuid, "consecrate")

		rn.item.equip(uuid, "radiant_crown")
		rn.item.equip(uuid, "radiant_platebody")
		rn.item.equip(uuid, "radiant_platelegs")
		rn.item.equip(uuid, "radiant_greatsword")
		rn.entity.prefabs.faction.set_faction(uuid, faction.player_enemy)
	end,
	on_cast_success = function(uuid, spellname, castx, casty)
		if spellname == "flash_of_light" then
			rn.entity.prefabs.spell_slots.equip_spell(uuid, "sacred_shield", true, "yellow")
		elseif spellname == "sacred_shield" then
			rn.entity.prefabs.spell_slots.equip_spell(uuid, "flash_of_light", true, "yellow")
		end
	end,
	update = rn.mods.basegame.prefabs.melee_monster.update,
	on_struck = rn.mods.basegame.prefabs.melee_monster.on_struck,
	on_move = rn.mods.basegame.prefabs.melee_monster.on_move,
	on_stop_moving = rn.mods.basegame.prefabs.melee_monster.on_stop_moving,
	on_collision = rn.mods.basegame.prefabs.melee_monster.on_collision,
	on_cast_begin = rn.mods.basegame.prefabs.melee_monster.on_cast_begin,
	on_death = rn.mods.basegame.prefabs.melee_monster.on_death,
	on_equip = rn.mods.basegame.prefabs.melee_monster.on_equip,
	on_unequip = rn.mods.basegame.prefabs.melee_monster.on_unequip,
}