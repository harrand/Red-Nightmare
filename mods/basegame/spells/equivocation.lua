rn.mods.basegame.spells.equivocation =
{
	cast_duration = 5.0,
	magic_type = "shadow",
	two_handed = true,
	slot = "red",
	cast_type = "omni",
	cooldown = 120.0,
	description = "Enact a shadowy ritual, weaving together a horde of truths and lies to swarm your foes.",
	finish = function(uuid, casterx, castery)
		rn.current_scene():entity_write(uuid, "equivocate_count", 0)
	end,
	advance = function(uuid, cast_progress)
		-- this is a two-handed shadow spell, but i want one hand to be holy.
		local buildup_right_hand = rn.current_scene():entity_read(uuid, "cast_buildup1")
		if buildup_right_hand ~= nil then
			rn.current_scene():entity_write(buildup_right_hand, "magic_type", "holy")	
		end

		local count = rn.current_scene():entity_read(uuid, "equivocate_count") or 0
		local total_summon_count = 4
		if count < ((cast_progress - 0.5) * (total_summon_count * 2)) then
			count = count + 1
			local xpos, ypos = rn.entity.prefabs.sprite.get_position(uuid)
			if count % 2 ~= 0 then
				rn.mods.basegame.spells.cruel_lie.finish(uuid, xpos, ypos)
			else
				rn.mods.basegame.spells.harsh_truth.finish(uuid, xpos, ypos)
			end
			rn.current_scene():entity_write(uuid, "equivocate_count", count)
		end
	end,
}