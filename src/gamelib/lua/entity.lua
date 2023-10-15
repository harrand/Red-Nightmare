rn.model =
{
	quad = 0,
	humanoid = 1
}

rn.faction =
{
	"pure friend",
	"pure enemy",
	"pure neutral",
	"player ally",
	"player enemy"
}

rn.faction_id =
{
	pure_friend = 0,
	pure_enemy = 1,
	pure_neutral = 2,
	player_ally = 3,
	player_enemy = 4
}

rn.relationship =
{
	"friendly",
	"neutral",
	"hostile"
}

rn.entity = {}
rn.entity.type = {}
rn.entity.living = {}
rn.entity.data = {}

rn.entity_handler = {}

rn.get_faction = function(ent)
	return rn.faction[ent:get_faction() + 1]
end

rn.get_relationship = function(ent1, ent2)
	return rn.relationship[ent1:get_relationship(ent2) + 1]
end

-- Include all entities here.
require "entity0"
require "entity1"
require "entity2"
require "entity3"

rn.entity_static_init = function()
	tracy.ZoneBegin()
	for i in pairs(rn.entity_handler) do
		local h = rn.entity_handler[i]
		if h.static_init ~= nil then
			h.static_init()
		end
	end
	tracy.ZoneEnd()
end

-- called early on in initialisation - before the scene element is attached
-- this is your opportunity to choose which model the entity will use.
-- note: as the scene element doesn't exist, you cannot use `ent:get_element()`
-- at all here.
rn.entity_preinit = function(type)
	-- assume variable exists in global "rn_impl_new_entity"!
	tracy.ZoneBegin()
	tz.assert(rn_impl_new_entity ~= nil)
	local ent = rn_impl_new_entity

	local handler = rn.entity_handler[type]
	tz.assert(handler ~= nil)
	if handler.preinit ~= nil then
		handler.preinit(ent)
	end
	rn.entity.living[ent:uid()] = true
	tracy.ZoneEnd()
end

-- called straight after the entity has finished initialisation. you can
-- do pretty much whatever you want at this point.
rn.entity_postinit = function(type)
	tracy.ZoneBegin()
	tz.assert(rn_impl_new_entity ~= nil)
	local ent = rn_impl_new_entity
	ent:set_health(ent:get_stats():get_maximum_health())

	local mod = ent:get_model()
	if mod == rn.model.humanoid then
		local e = ent:get_element()
		e:object_set_visibility(4, false)
		e:object_set_visibility(6, false)
	end

	local handler = rn.entity_handler[type]
	tz.assert(handler ~= nil)
	if handler.postinit ~= nil then
		handler.postinit(ent)
	end
	tracy.ZoneEnd()
end

rn.entity_update = function(ent)
	tracy.ZoneBegin()
	tz.assert(ent ~= nil)
	local handler = rn.entity_handler[ent:get_type()]
	tz.assert(handler ~= nil)
	if handler.update ~= nil then
		handler.update(ent)
	end
	tracy.ZoneEnd()
end

rn.entity_deinit = function()
	-- assume variable exists in global `rn_impl_dead_entity`
	tz.assert(rn_impl_dead_entity ~= nil)
	local uid = rn_impl_dead_entity:uid()
	rn.entity.living[uid] = false
	rn.entity.data[uid] = {}
end

rn.internal_key_state = {}

rn.for_each_collision = function(ent, fn)
	local collision_count = rn.scene():get_collision_count(ent:uid())
	if collision_count == 0 then
		return false
	end
	for i=0,collision_count-1,1 do
		local colliding_entity_id = rn.scene():get_collision(ent:uid(), i)
		local colliding_entity = rn.scene():get(colliding_entity_id)
		fn(colliding_entity)
	end
	return true
end

rn.empty_key_state = function()
	for i=0,tz.wsi_key_count-2,1 do
		rn.internal_key_state[i] = false
	end
end

rn.is_key_down = function(name)
	if rn.internal_key_names == nil then
		rn.internal_key_names = {}
		for i=0,tz.wsi_key_count-2,1 do
			local str = tz.window():get_key_name(i)
			rn.internal_key_names[tz.window():get_key_name(i)] = i
		end
	end

	local key_id = rn.internal_key_names[name]
	local ret = rn.internal_key_state[key_id]
	return rn.internal_key_state[key_id]
end

rn.advance_key_state = function()
	local wnd = tz.window()
	for i=0,tz.wsi_key_count-2,1 do
		local val = wnd:is_key_id_down(i)
		rn.internal_key_state[i] = val
	end
end

rn.update = function()
	tracy.ZoneBegin()

	rn.empty_key_state()
	rn.advance_key_state()

	local sc = rn.scene()
	if sc:size() > 0 then
		for i=0,sc:size()-1,1 do
			local ent = sc:get(i)
			if rn.entity.living[ent:uid()] == true then
				rn.entity_update(ent)
			end
		end
	end
	tracy.ZoneEnd()
end