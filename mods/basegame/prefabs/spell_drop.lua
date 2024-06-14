rn.mods.basegame.prefabs.spell_drop =
{
	description = "Meta Entity representing a dropped spell",
	static_init = function()
		rn.renderer():add_texture("icon.border", "basegame/res/icons/icon_border.png")
	end,
	pre_instantiate = rn.mods.basegame.prefabs.sprite.pre_instantiate,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_scale(uuid, 0.75)
	end,
	update = function(uuid, delta_seconds)
		local spellname = rn.entity.prefabs.spell_drop.get_spell(uuid)
		if spellname == nil then return end
		local spelldata = rn.spell.spells[spellname]
		local icon = spelldata.icon or "invisible"
		local colour = rn.spell.slot[spelldata.slot].light_colour

		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "timer") or 0.0
		t = t + delta_seconds * 2
		sc:entity_write(uuid, "timer", t)

		local blacklist_timer = sc:entity_read(uuid, "spellpickup_blacklist_timer") or 0.0
		-- after 2.5 seconds have passed, the blacklist is automatically cleared
		-- note 5.0 = 2.5 * 2 coz timer moves at double speed.
		if blacklist_timer ~= nil and t > blacklist_timer + 5.0 then
			rn.entity.prefabs.spell_drop.clear_blacklisted(uuid)
		end

		local x, y = rn.entity.prefabs.sprite.get_position(uuid)
		rn.entity.prefabs.sprite.set_position(uuid, x, y + 0.01 * math.sin(t))
		rn.entity.prefabs.sprite.set_rotation(uuid, -1.5708)
		rn.entity.prefabs.sprite.set_texture(uuid, icon)
		rn.entity.prefabs.sprite.set_emissive_map(uuid, "icon.border")
		rn.entity.prefabs.sprite.set_emissive_tint(uuid, colour[1], colour[2], colour[3])

	end,
	get_position = rn.mods.basegame.prefabs.sprite.get_position,
	set_position = rn.mods.basegame.prefabs.sprite.set_position,
	on_collision = function(me, other)
		-- only the player can pickup a spell. would be ridiculous otherwise.
		local should_equip_me = rn.entity.prefabs.combat_stats.is_alive(other) and rn.current_scene():entity_read(me, "spellpickup_blacklist") ~= other
		local spellname = rn.entity.prefabs.spell_drop.get_spell(me)
		if not should_equip_me or spellname == nil or (other ~= rn.player.get())then
			return false
		end

		rn.entity.prefabs.spell_slots.equip_spell(other, spellname)
		rn.current_scene():remove_entity(me)
		return false
	end,
	clear_blacklisted = function(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "spellpickup_blacklist_timer", nil)
		sc:entity_write(uuid, "spellpickup_blacklist", nil)
	end,
	set_blacklisted = function(uuid, other_uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "spellpickup_blacklist_timer", sc:entity_read(uuid, "timer") or 0.0)
		sc:entity_write(uuid, "spellpickup_blacklist", other_uuid)
	end,
	set_spell = function(uuid, spellname)
		rn.current_scene():entity_write(uuid, "spell_name", spellname)
	end,
	get_spell = function(uuid)
		return rn.current_scene():entity_read(uuid, "spell_name")
	end
}