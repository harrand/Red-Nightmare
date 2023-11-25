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
rn.entity.resident = {}
rn.entity.data = {}
rn.player_credits = 0

rn.entity_handler = {}

rn.entity_distance = function(ent, ent2)
	local x, y = ent:get_element():get_position()
	local x2, y2 = ent2:get_element():get_position()
	local xdiff = x2 - x
	local ydiff = y2 - y
	return math.sqrt(xdiff*xdiff + ydiff*ydiff)
end

rn.datastore_internal_write_table = function(table_name, tbl)
	local ds = rn.data_store()
	for k, v in pairs(tbl) do
		local name = table_name .. "." .. k
		if type(v) == 'table' then
			rn.datastore_internal_write_table(name, v)
		else
			ds:add(name, v)
		end
	end
end

rn.entity_data_read = function(ent, ...)
	local args = table.pack(...)
	local amended_args = {}
	for i,key in pairs(args) do
		if type(i) == 'number' then
			key = "ent." .. string.format("%.0f", ent:uid()) .. ".data." .. key
			table.insert(amended_args, key)
		end
	end
	return rn.data_store():read_some(table.unpack(amended_args))
end

rn.entity_data_write = function(ent, ...)
	-- args: ent, {key1, value1, key2, value2}
	-- so if even, we're a key
	local args = table.pack(...)
	local amended_args = {}
	local counter=0
	for i,key_or_val in pairs(args) do
		if counter % 2 == 0 then
			-- even. we're a key and we want to amend our name
			key_or_val = "ent." .. string.format("%.0f", ent:uid()) .. ".data." .. key_or_val
		end
		counter = counter + 1
		table.insert(amended_args, key_or_val)
	end
	rn.data_store():edit_some(table.unpack(amended_args))
end

rn.entity_data_clear = function(ent)
	tracy.ZoneBegin()
	rn.data_store():remove_all_of("ent." .. string.format("%.0f", ent:uid()) .. ".data")
	tracy.ZoneEnd()
end

rn.impl_entity_entity_valid_target = function(ent, args, ent2)
	local aggro_range = args.aggro_range or 12
	local can_target_dead = false
	if args.can_target_dead ~= nil then
		can_target_dead = args.can_target_dead
	end
	if not ent2:is_valid() then
		return false
	end
	if ent2:is_dead() then
		if can_target_dead == false and not args.only_target_dead then
			return false
		end
	else
		-- not dead
		if args.only_target_dead then
			return false
		end
	end
	if args.target_relationship ~= nil then
		if rn.get_relationship(ent, ent2) ~= args.target_relationship then
			return false
		end
	end
	local targetable, projectile_skip, undead = rn.entity_data_read(ent2, "impl.targetable", "impl.projectile_skip", "impl.undead")
	if targetable == false or projectile_skip then
		return false
	end
	if args.no_undead and undead then
		return false
	end
	if rn.entity_distance(ent, ent2) > aggro_range then
		return false
	end
	return true
end

rn.entity_target_entity = function(ent, args)
	-- find a new target
	for i=1,rn.scene():size()-1,1 do
		local ent2 = rn.scene():get(i)
		if rn.impl_entity_entity_valid_target(ent, args, ent2) then
			return ent2
		end
	end
	return nil
end

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
require "entity4"
require "entity5"
require "entity6"
require "entity7"
require "entity8"
require "entity9"
require "entity10"
require "entity11"
require "entity12"
require "entity13"
require "entity14"
require "entity15"

rn.get_entity_type_name = function()
	_tmp_type_name = nil
	for i in pairs(rn.entity.type) do
		local id = rn.entity.type[i]
		if id == _tmp_type_id then
			_tmp_type_name = i
		end
	end
end

rn.get_entity_type_count = function()
	_tmp_type_count = 0
	for _, _ in pairs(rn.entity_handler) do
		_tmp_type_count = _tmp_type_count + 1
	end
end

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
	rn.entity.resident[ent:uid()] = true
	tracy.ZoneEnd()
end

-- called straight after the entity has finished initialisation. you can
-- do pretty much whatever you want at this point.
rn.entity_postinit = function(type)
	local o <close> = tz.create_profiling_object()
	tz.assert(rn_impl_new_entity ~= nil)
	local ent = rn_impl_new_entity
	ent:set_health(ent:get_stats():get_maximum_health())

	local mod = ent:get_model()
	local e = ent:get_element()
	if mod == rn.model.humanoid then
		e:object_set_visibility(5, false)
		e:object_set_visibility(7, false)
		e:face_forward()
		rn.entity_get_data(ent).impl.dir = "forward"
	elseif mod == rn.model.quad then
		e:object_set_visibility(2, true)
		e:face_forward2d()
		e:rotate(1.5708)
	end

	local handler = rn.entity_handler[type]
	tz.assert(handler ~= nil)
	if handler.postinit ~= nil then
		handler.postinit(ent)
	end
end

rn.entity_update = function(ent)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text("Entity Update - " .. ent:get_name())
	obj:set_name("Lua Entity Update")

	tz.assert(ent ~= nil)
	local data = rn.entity_get_data(ent)
	if data.impl.trivial then
		return
	end

	data.impl.is_moving = false

	tracy.ZoneBeginN("Entity Handler Overhead")
	local handler = rn.entity_handler[ent:get_type()]
	tracy.ZoneEnd()
	tz.assert(handler ~= nil)
	if handler.update ~= nil then
		local obj2 <close> = tz.profzone_obj:new()
		obj2:set_name("Update " .. ent:get_name() .. " (type " .. ent:get_type() .. ")")
		handler.update(ent)
	end

	-- deal with casts.
	local casting = rn.entity_data_read(ent, "impl.is_casting")
	if casting == true then
		-- is the cast finished?
		rn.casting_advance(ent)
	end

	if not ent:is_valid() then
		return
	end

	local e = ent:get_element()
	if not ent:is_dead() and not casting and not data.impl.is_moving then
		if (ent:get_model() == rn.model.humanoid) and (e:get_playing_animation_name() ~= "CastIdle" or not e:is_animation_playing()) then
			e:play_animation_by_name("CastIdle", false)
		end
	end

	-- deal with despawning dead entities.
	if ent:is_dead() then
		-- if entity didnt specify a custom despawn timer, default to 45 seconds.
		local timer = rn.entity_data_read(ent, "impl.custom_despawn_timer")
		if timer == nil then
			timer = 45000
			rn.entity_data_write(ent, "impl.custom_despawn_timer", timer)
		end
		-- if custom despawn timer is -1, it never despawns.
		-- if it is though... let's despawn it if it needs to
		local death_time = rn.entity_data_read(ent, "impl.death_time")
		if timer ~= -1 and death_time ~= nil then
			local now = tz.time()
			if death_time + timer <= tz.time() then
				rn.scene():remove_uid(ent:uid())
			end
		end

	end

	-- deal with the equipment buff.
	local eqbuff = rn.new_buff()
	for i=1,rn.equipment.slot._count-1,1 do
		local eq = rn.get_equipped_item(ent, i)
		if eq ~= nil then
			local itemdata = rn.items[eq]
			local item_equipment_buff = itemdata:get_buff()
			if item_equipment_buff ~= nil then
				eqbuff = eqbuff:combine(item_equipment_buff)
			end
		end
	end
	ent:impl_set_equipment_buff(eqbuff)
end

rn.entity_deinit = function()
	-- assume variable exists in global `rn_impl_dead_entity`
	tz.assert(rn_impl_dead_entity ~= nil)

	local handler = rn.entity_handler[rn_impl_dead_entity:get_type()]
	tz.assert(handler ~= nil)
	if handler.deinit ~= nil then
		handler.deinit(rn_impl_dead_entity)
	end

	local uid = rn_impl_dead_entity:uid()
	rn.entity.resident[uid] = false
	rn.entity.data[uid] = nil
	rn.entity_data_clear(rn_impl_dead_entity)
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
		local ret = fn(colliding_entity)
		if ret ~= nil and ret == false then
			return true
		end
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

rn.entity_get_data = function(ent)
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("entity_get_data")
	rn.entity.data[ent:uid()] = rn.entity.data[ent:uid()] or {}
	rn.entity.data[ent:uid()].impl = rn.entity.data[ent:uid()].impl or {}
	return rn.entity.data[ent:uid()]
end

rn.update = function()
	local obj <close> = tz.profzone_obj:new()
	obj:set_name("Lua Update")

	rn.empty_key_state()
	rn.advance_key_state()

	local sc = rn.scene()
	if sc:size() > 0 then
		for i=0,sc:size()-1,1 do
			local ent = sc:get(i)
			if rn.entity.resident[ent:uid()] == true and ent:is_valid() then
				rn.entity_update(ent)
			end
		end
	end
end

rn.entity_move_to_entity = function(arg, ent2)
	local locx, locy = ent2:get_element():get_position()
	rn.entity_move_to_location(arg, locx, locy)
end

rn.entity_move_to_location = function(arg, locx, locy)
	local ent = arg.ent or nil
	tz.assert(ent ~= nil)
	local ourx, oury = ent:get_element():get_position()
	local vecx = locx - ourx
	local vecy = locy - oury
	local speed = ent:get_stats():get_movement_speed() * rn.delta_time * 0.5

	local new_dir = {}
	if vecx >= speed then
		-- move right
		table.insert(new_dir, "right")
	elseif vecx <= -speed then
		-- move left
		table.insert(new_dir, "left")
	end
	if vecy >= speed then
		-- move up
		table.insert(new_dir, "backward")
	elseif vecy <= -speed then
		-- move down
		table.insert(new_dir, "forward")
	end
	-- if we're not on them, move
	if not rawequal(next(new_dir), nil) then
		rn.entity_move{ent = ent, dir = new_dir, movement_anim_name = arg.movement_anim_name}
	end

	if math.abs(vecy) > math.abs(vecx) then
		-- face vertically if we need to (entity_move default face direction is bad)
		if vecy > 0 then
			ent:get_element():face_backward()
		else
			ent:get_element():face_forward()
		end
	end
end

rn.entity_move = function(arg)
	local ent = arg.ent or nil
	tz.assert(ent ~= nil)
	local dir = arg.dir
	-- dir needs to be a table. if its a string just convert it into a table with a string inside.
	if type(dir) == "string" then
		dir = {dir}
	end
	local vecdir_x = arg.vecdir_x
	local vecdir_y = arg.vecdir_y
	local movement_anim_name = arg.movement_anim_name
	local face_in_direction = arg.face_in_direction
	if face_in_direction == nil then face_in_direction = true end

	local entdata = rn.entity_get_data(ent)
	entdata.impl = entdata.impl or {}

	local e = ent:get_element()
	-- get normalised movement vector
	local xdiff = 0
	local ydiff = 0
	if vecdir_x == nil or vecdir_y == nil then
		for i, d in pairs(dir) do
			-- move in an axis-aligned direction
			if d == "forward" then
				ydiff = ydiff - 1
			elseif d == "backward" then
				ydiff = ydiff + 1
			elseif d == "right" then
				xdiff = xdiff + 1
			elseif d == "left" then
				xdiff = xdiff - 1
			else
				tz.assert(false)
			end
		end
	else
		-- move in direction of an arbitrary angle
		xdiff = -vecdir_x
		ydiff = -vecdir_y
	end

	-- set face direction
	if face_in_direction then
		if xdiff == 0 then
			if ydiff > 0 then
				e:face_backward()
				entdata.impl.dir = "backward"
			elseif ydiff < 0 then
				e:face_forward()
				entdata.impl.dir = "forward"
			end
		elseif xdiff > 0 then
			e:face_right()
			entdata.impl.dir = "right"
		elseif xdiff < 0 then
			e:face_left()
			entdata.impl.dir = "left"
		end
	end

	if (xdiff ~= 0 or ydiff ~= 0) and not rn.entity_data_read(ent, "impl.is_casting") then
		-- do movement
		entdata.impl.is_moving = true
		local x, y = e:get_position()
		local hypot = math.sqrt(xdiff*xdiff + ydiff*ydiff)
		xdiff = xdiff / hypot
		ydiff = ydiff / hypot
		local stats = ent:get_stats()
		local movement_speed = stats:get_movement_speed()
		x = x + xdiff * movement_speed * rn.delta_time
		y = y + ydiff * movement_speed * rn.delta_time
		e:set_position(x, y)
		e:set_animation_speed(math.sqrt(movement_speed / 3.0))

		if movement_anim_name ~= nil and (e:get_playing_animation_name() ~= movement_anim_name or not e:is_animation_playing()) then
			e:play_animation_by_name(movement_anim_name, false)
		end
	end

end