rn.combat = {}
rn.entity_damage_entity_event = 
{
	tag = "entity_damage_entity",
	damager = nil,
	damagee = nil,
	value = 0,
	damage_type = "Physical"
}

function rn.entity_damage_entity_event:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function rn.combat.base_on_death(ent, evt)
	print(ent:get_name() .. "oh noes i dieded :(")
	ent:get_element():play_animation(7, false)
end

-- ent was hit by a entity_damage_entity_event
function rn.combat.base_on_struck(ent, evt)
	local hp = ent:get_health()
	local new_hp = ent:get_health() - evt.value
	if new_hp <= 0 then
		new_hp = 0
		if not ent:is_dead() then
			rn.combat.base_on_death(ent, evt)
		end
	end
	ent:set_health(new_hp)
end

function rn.combat.base_on_hit(ent, dmg)
	-- bleh
end

function rn.combat.process_event(evt)
	tz.assert(evt ~= nil)
	tz.assert(evt.tag ~= nil)
	if evt.tag == "entity_damage_entity" then
		local damager = rn.scene():get_uid(evt.damager)
		local damagee = rn.scene():get_uid(evt.damagee)
		print(damager:get_name() .. " hurts " .. damagee:get_name() .. " for " .. evt.value .. " damage.")
		rn.combat.base_on_struck(damagee, evt)
		rn.combat.base_on_hit(damager, evt)
	end
end