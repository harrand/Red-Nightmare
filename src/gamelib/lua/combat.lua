rn.combat = {}
rn.entity_damage_entity_event = 
{
	tag = "entity_damage_entity",
	damager = nil,
	damagee = nil,
	value = 0,
	damage_type = "Physical"
}

rn.damage_type_get_colour = function(damage_type)
	if damage_type == "Physical" then return nil, nil, nil end

	if damage_type == "Fire" then return 1.0, 0.4, 0.1 end
	if damage_type == "Frost" then return 0.1, 0.4, 1.0 end
	--if damage_type == "Shadow" then return 0.2, 0.0, 0.4 end
	if damage_type == "Shadow" then return -0.17, -0.15, -0.17 end
	if damage_type == "Anthir" then return 0.2, 0.0, 0.4 end
	if damage_type == "Divine" then return 1.0, 0.85, 0.4 end

	return 0.0, 0.0, 0.0
end

function rn.entity_damage_entity_event:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

rn.entity_heal_entity_event = 
{
	tag = "entity_heal_entity",
	healer = nil,
	healee = nil,
	value = 0,
}

function rn.entity_heal_entity_event:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function rn.combat.base_on_death(ent, evt)
	-- call on_death if it exists
	local handler = rn.entity_handler[ent:get_type()]
	if handler.on_death ~= nil then
		local success = handler.on_death(ent, evt)
		if success ~= nil and not success then return end
	end
	rn.unequip_and_drop_all(ent)
	rn.cancel_cast(ent)
	ent:get_element():play_animation_by_name("Death", false)
	rn.entity_data_write(ent, "impl.death_time", tz.time())
end

function rn.combat.base_on_kill(ent, evt)
	-- call on_kill if it exists
	local handler = rn.entity_handler[ent:get_type()]
	if handler.on_kill ~= nil then
		handler.on_kill(ent, evt)
	end
end

-- ent was hit by a entity_damage_entity_event
function rn.combat.base_on_struck(ent, evt)
	local hp = ent:get_health()
	local new_hp = ent:get_health() - evt.value
	local damager = rn.scene():get_uid(evt.damager)

	-- call on_struck if it exists
	local handler = rn.entity_handler[ent:get_type()]
	tz.assert(handler ~= nil)
	if handler.on_struck ~= nil then
		local success = handler.on_struck(ent, evt)
		if success ~= nil and not success then return end
	end

	if new_hp <= 0 then
		new_hp = 0
		if not ent:is_dead() then
			rn.combat.base_on_death(ent, evt)
			rn.combat.base_on_kill(damager, evt)
		end
	end
	tz.assert(handler ~= nil)
	ent:set_health(new_hp)

	-- blood splatter
	local splatter = rn.scene():get(rn.scene():add(6))
	local desired_subobject = fakenil
	if ent:get_model() == rn.model.humanoid then
		desired_subobject = 9
	end
	rn.entity_data_write(splatter, "subobject", desired_subobject, "target_entity", ent:uid(), "damage_type", evt.damage_type, "duration", 500)
end

-- ent was affected by a entity_heal_entity_event
function rn.combat.base_on_healed(ent, evt)
	local hp = ent:get_health()
	local max = ent:get_stats():get_maximum_health()
	local hpdiff = max - (hp + evt.value)
	if hpdiff < 0 then
		evt.value = evt.value + hpdiff
	end
	local new_hp = hp + evt.value
	-- call on_healed if it exists
	local handler = rn.entity_handler[ent:get_type()]
	if handler.on_healed ~= nil then
		local success = handler.on_healed(ent, evt)
		if success ~= nil and not success then return end
	end
	ent:set_health(new_hp)
end

function rn.combat.base_on_hit(ent, evt)
	-- call on_hit if it exists
	local handler = rn.entity_handler[ent:get_type()]
	if handler.on_hit ~= nil then
		local success = handler.on_hit(ent, evt)
		if success ~= nil and not success then return end
	end
end

function rn.combat.base_on_heal(ent, evt)
	-- call on_heal if it exists
	local handler = rn.entity_handler[ent:get_type()]
	if handler.on_heal ~= nil then
		local success = handler.on_heal(ent, evt)
		if success ~= nil and not success then return end
	end
end

function rn.combat.process_damage_mitigation(evt)
	if evt.damage_type == "Physical" then
		-- mit by defence.
		local damagee = rn.scene():get_uid(evt.damagee)
		local damager = rn.scene():get_uid(evt.damager)
		local victim_defence_rating = damagee:get_stats():get_defence_rating()
		-- DR% = Armor / (Armor + 400 + 85 * AttackerLevel)
		local dr = victim_defence_rating / (victim_defence_rating + 40 + 15 * damager:get_level())
		evt.value = math.ceil(evt.value * (1.0 - dr));
	end
	return evt
end

function rn.combat.process_event(evt)
	tz.assert(evt ~= nil)
	tz.assert(evt.tag ~= nil)
	if evt.tag == "entity_damage_entity" then
		local damager = rn.scene():get_uid(evt.damager)
		local damagee = rn.scene():get_uid(evt.damagee)
		evt = rn.combat.process_damage_mitigation(evt)
		rn.combat.base_on_struck(damagee, evt)
		rn.combat.base_on_hit(damager, evt)
		local ability_name = rn.entity_data_read(damager, "impl.cast")
		local ability_str = ""
		if ability_name ~= nil then
			ability_str = " with " .. ability_name 
		end
		--print(damager:get_name() .. " hurts " .. damagee:get_name() .. ability_str .. " for " .. evt.value .. " " .. evt.damage_type .. ".")
	end
	if evt.tag == "entity_heal_entity" then
		local healer = rn.scene():get_uid(evt.healer)
		local healee = rn.scene():get_uid(evt.healee)
		rn.combat.base_on_healed(healee, evt)
		rn.combat.base_on_heal(healer, evt)
	end
end