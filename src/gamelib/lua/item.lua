rn.item =
{
	id = 0,
	name = "Unknown",
	slot_id = nil,
	texture_name = nil
}
rn.items = {}

function rn.item:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function rn.item:get_id()
	return self.id
end

function rn.item:set_id(id)
	self.id = id
end

function rn.item:get_name()
	return self.name
end

function rn.item:set_name(name)
	self.name = name
end

function rn.item:get_equipment_slot_id()
	return self.slot_id
end

function rn.item:set_equipment_slot_id(id)
	self.slot_id = id
end

function rn.item:get_texture_name()
	return self.texture_name
end

function rn.item:set_texture_name(name)
	self.texture_name = name
end

rn.items = {}

require "item0"

rn.item_static_init = function()
	for k, cur_item in pairs(rn.items) do
		-- item static init
		local path = cur_item:get_texture_name()
		local name = cur_item:get_name()
		tz.assert(path ~= nil)
		print(path)
		rn.texture_manager():register_texture(name .. ".texture", path)
	end
end