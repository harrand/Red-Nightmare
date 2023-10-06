rn.model =
{
	quad = 0,
	humanoid = 1
}

rn.entity = {}
rn.entity.type = {}

rn.entity_handler = {}

-- Include all entities here.
require "entity0"
require "entity1"

-- called early on in initialisation - before the scene element is attached
-- this is your opportunity to choose which model the entity will use.
-- note: as the scene element doesn't exist, you cannot use `ent:get_element()`
-- at all here.
rn.entity_preinit = function(type)
	-- assume variable exists in global "rn_impl_new_entity"!
	tz.assert(rn_impl_new_entity ~= nil)
	local ent = rn_impl_new_entity

	local handler = rn.entity_handler[type]
	tz.assert(handler ~= nil)
	if handler.preinit ~= nil then
		handler.preinit(ent)
	end
end

-- called straight after the entity has finished initialisation. you can
-- do pretty much whatever you want at this point.
rn.entity_postinit = function(type)
	tz.assert(rn_impl_new_entity ~= nil)
	local ent = rn_impl_new_entity

	local handler = rn.entity_handler[type]
	tz.assert(handler ~= nil)
	if handler.postinit ~= nil then
		handler.postinit(ent)
	end
	ent:get_element():play_animation(1, false)
end

rn.entity_update = function(ent)
	tz.assert(ent ~= nil)
	local handler = rn.entity_handler[ent:get_type()]
	tz.assert(handler ~= nil)
	if handler.update ~= nil then
		handler.update(ent)
	end
end

rn.update = function()
	sc = rn.scene()
	if sc:size() > 0 then
		for i=0,sc:size()-1,1 do
			ent = sc:get(i)
			rn.entity_update(ent)
		end
	end
end