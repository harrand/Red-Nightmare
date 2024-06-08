rn = rn or {}
rn.buff = rn.buff or {}
rn.buff.buffs = rn.buff.buffs or {}

rn.buff.data =
{
	name = "Default Buff",
	on_apply = nil,
	on_update = nil,
}

rn.buff.prefix = function(buff_name, postfix)
	if postfix ~= nil then
		return "buff." .. buff_name .. postfix
	else
		return "buff." .. buff_name
	end
end

rn.buff.apply = function(uuid, buff_name)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text(tostring(uuid))
	obj:set_name("Apply Buff - \"" .. buff_name .. "\"")
	local sc = rn.current_scene()

	if rn.buff.is_applied(uuid, buff_name) then
		-- dont reapply the same buff.
		return
	end

	local buffdata = rn.buff.buffs[buff_name]
	sc:entity_write(uuid, rn.buff.prefix(buff_name, "duration"), buffdata.duration)
	sc:entity_apply_buff(uuid, buff_name)
	if buffdata.on_apply ~= nil then
		buffdata.on_apply(uuid)
	end
end

rn.buff.remove = function(uuid, buff_name)
	local buffdata = rn.buff.buffs[buff_name]
	if buffdata.on_remove ~= nil then
		buffdata.on_remove(uuid)
	end
	rn.current_scene():entity_remove_buff(uuid, buff_name)
end

rn.buff.clear_all = function(uuid)
	local sc = rn.current_scene()
	if uuid == nil or not sc:contains_entity(uuid) then return end
	local buff_count = sc:entity_get_buff_count(uuid)
	if buff_count == 0 then return end
	for i=0,buff_count - 1,1 do
		local buff_name = sc:entity_get_buff(uuid, i)
		rn.buff.remove(uuid, buff_name)
	end
end

rn.buff.is_applied = function(uuid, buff_name)
	local buff_exists = false
	rn.buff.iterate_buffs(uuid, function(buff_name2)
		if buff_name == buff_name2 then
			buff_exists = true
		end
	end)
	return buff_exists
end

rn.buff.advance = function(uuid, buff_name, delta_seconds)
	local sc = rn.current_scene()
	local duration = sc:entity_read(uuid, rn.buff.prefix(buff_name, "duration"))
	if duration ~= nil then
		duration = duration - delta_seconds
		if duration > 0.0 then
			sc:entity_write(uuid, rn.buff.prefix(buff_name, "duration"), duration)
		else
			rn.buff.remove(uuid, buff_name)
			return
		end
	end

	local buffdata = rn.buff.buffs[buff_name]
	if buffdata.on_advance ~= nil then
		buffdata.on_advance(uuid, delta_seconds)
	end
end

rn.buff.advance_buffs = function(uuid, delta_seconds)
	-- we need to advance all buffs.
	-- we can either: iterate over every buff in the game, and if its applied, advance it (slow)
	-- OR, only iterate over the buffs we know the entity has.
	-- this is not trivial, so we add c++ side support for this.
	rn.buff.iterate_buffs(uuid, function(buff_name)
		rn.buff.advance(uuid, buff_name, delta_seconds)
	end)
end

rn.buff.iterate_buffs = function(uuid, fn)
	local sc = rn.current_scene()
	if uuid == nil or not sc:contains_entity(uuid) then return end
	local buff_count = sc:entity_get_buff_count(uuid)
	if buff_count == 0 then return end
	for i=0,buff_count - 1,1 do
		local buff_name = sc:entity_get_buff(uuid, i)
		fn(buff_name)
	end
end