rn.mods.basegame.prefabs.spellsteal_projectile =
{
	description = "Steals the next spell cast by the victim",
	pre_instantiate = rn.mods.basegame.prefabs.magic_ball_base.pre_instantiate,
	instantiate = function(uuid)
		rn.mods.basegame.prefabs.magic_ball_base.instantiate(uuid)
		rn.current_scene():entity_write(uuid, "magic_type", "shadow")

		-- todo: saner magic colours
		local colour = rn.spell.schools.shadow.colour
		rn.entity.prefabs.sprite.set_colour(uuid, colour[1], colour[2], colour[3])
	end,
	update = rn.mods.basegame.prefabs.magic_ball_base.update,
	on_collision = function(me, other)
		-- special behaviour:
		-- if the magicbolt hits a dropped itemset containing an elemental circlet - it ignites it.
		local target_alive = rn.entity.prefabs.combat_stats.is_alive(other)
		local owner_id = rn.current_scene():entity_read(me, "owner")
		local other_is_projectile = rn.current_scene():entity_read(other, ".is_projectile")
		-- if we touch an obstacle, die.
		if rn.entity.prefabs.obstacle.is_obstacle(other) then
			rn.current_scene():remove_entity(me)
			return false
		end
		if owner_id == other or not target_alive or (owner_id ~= nil and owner_id == other_owner) or rn.entity.prefabs.faction.is_ally(me, other) or other_is_projectile then
			-- collided with whomsoever casted me. don't do anything.
			return false
		end
		-- we touched someone.
		-- apply the spell_stolen_from debuff (with caster = our owner)
		rn.buff.apply(other, "spell_stolen_from")
		rn.current_scene():entity_write(other, "spell_thief", owner_id)
		rn.current_scene():remove_entity(me)
		return false
	end,
	on_remove = rn.mods.basegame.prefabs.magic_ball_base.on_remove
}