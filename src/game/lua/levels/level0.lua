local id = 0
local levelstr = "blanchfield"
rn.level.type[levelstr] = id

local spawn_tree = function(x, y)
	local ent = rn.scene():get(rn.scene():add(9))	
	ent:set_name("Tree")
	ent:get_element():object_set_texture_handle(2, 0, rn.texture_manager():get_texture("scenery.tree0"))
	ent:get_element():set_position(x, y)
	ent:get_element():set_uniform_scale(2.0)
	local bstats = ent:get_base_stats()
	bstats:set_maximum_health(999999)
	bstats:set_defence_rating(999999)
	ent:set_base_stats(bstats)
	ent:set_health(ent:get_stats():get_maximum_health())
	local entdata = rn.entity_get_data(ent)
	entdata.impl.targetable = false
end

local spawn_loot_chest = function(x, y)
	local ent = rn.scene():get(rn.scene():add(12))
	ent:set_name("Loot Chest")
	ent:get_element():set_position(x, y)
	local data = rn.entity_get_data(ent)
	
	local itemset = {}
	for k in pairs(rn.items) do
		table.insert(itemset, k)
	end

	local item_name = itemset[math.random(#itemset)]
	data.loot = item_name
	data.impl.targetable = false
end

local spawn_invisible_wall = function(x, y, sc)
	sc = sc or 1.0
	local ent = rn.scene():get(rn.scene():add(9))	
	ent:set_name("Invisible Wall")
	ent:get_element():set_position(x, y)
	ent:get_element():set_uniform_scale(2.0 * sc)
	ent:get_element():object_set_visibility(2, false)
	local bstats = ent:get_base_stats()
	bstats:set_maximum_health(999999)
	bstats:set_defence_rating(999999)
	ent:set_base_stats(bstats)
	ent:set_health(ent:get_stats():get_maximum_health())
	local entdata = rn.entity_get_data(ent)
	entdata.impl.targetable = false
	entdata.impl.projectile_skip = true
end

rn.level_handler[id] =
{
	on_load = function()
		rn.play_music("soundtrack1.mp3", 0, 0.8)
		for i=-63,63,6 do
			local y = math.random(0, 126) - 63
			spawn_tree(i, y)
		end

		for i=-63,63,12 do
			local y = math.random(0, 126) - 63
			spawn_loot_chest(i, y)
		end
		spawn_invisible_wall(-7.0, -7.0)
		spawn_invisible_wall(-7.0, -10.0)

		spawn_invisible_wall(-4.0, -4.0)
		spawn_invisible_wall(-4.0, -6.0)
		spawn_invisible_wall(-4.0, -9.0)
		spawn_invisible_wall(-4.0, -12.0)
		spawn_invisible_wall(-4.0, -15.0)
		spawn_invisible_wall(-4.0, -18.0)

		spawn_invisible_wall(2.0, -4.0)
		spawn_invisible_wall(2.0, -6.0)
		spawn_invisible_wall(2.0, -9.0)
		spawn_invisible_wall(2.0, -12.0)
		spawn_invisible_wall(2.0, -15.0)
		spawn_invisible_wall(2.0, -18.0)

		spawn_invisible_wall(5.0, -4.0)
		spawn_invisible_wall(8.0, -4.0)
		spawn_invisible_wall(11.0, -4.0)
		spawn_invisible_wall(12.0, -4.0)
		spawn_invisible_wall(15.0, -6.0)
		spawn_invisible_wall(18.0, -6.0)
		spawn_invisible_wall(21.0, -8.0)
		spawn_invisible_wall(22.0, -8.0)
		spawn_invisible_wall(25.0, -10.0)
		spawn_invisible_wall(26.0, -10.0)
		spawn_invisible_wall(29.0, -12.0)
		spawn_invisible_wall(32.0, -12.0)
		spawn_invisible_wall(34.0, -12.0)
		spawn_invisible_wall(36.0, -10.0)
		spawn_invisible_wall(39.0, -10.0)
		spawn_invisible_wall(42.0, -10.0)
		spawn_invisible_wall(45.0, -10.0)
		spawn_invisible_wall(47.0, -10.0)
		spawn_invisible_wall(50.0, -7.0)
		spawn_invisible_wall(50.0, -6.0)
		spawn_invisible_wall(51.0, -4.0)
		spawn_invisible_wall(54.0, -2.0)
		spawn_invisible_wall(54.0, 0.0)
		spawn_invisible_wall(54.0, 0.0)
		spawn_invisible_wall(55.0, 3.0, 0.5)
		spawn_invisible_wall(55.0, 2.0, 0.5)
		spawn_invisible_wall(57.0, 1.0, 0.5)
		spawn_invisible_wall(57.0, -1.0, 0.5)
		spawn_invisible_wall(54.0, -5.0)
		spawn_invisible_wall(54.0, -6.0)
		spawn_invisible_wall(54.0, -7.5)
		spawn_invisible_wall(56.0, -10.0)
		spawn_invisible_wall(55.0, -12.0, 0.5)
		spawn_invisible_wall(55.0, -13.0, 0.5)
		spawn_invisible_wall(53.0, -15.0, 0.5)
		spawn_invisible_wall(51.0, -17.0, 0.5)
		spawn_invisible_wall(48.0, -18.0)
		spawn_invisible_wall(45.0, -18.0)
		spawn_invisible_wall(42.0, -18.0)
		spawn_invisible_wall(39.0, -18.0)
		spawn_invisible_wall(37.0, -18.0)
		spawn_invisible_wall(34.0, -20.0)
		spawn_invisible_wall(31.0, -20.0)
		spawn_invisible_wall(28.0, -20.0)
		spawn_invisible_wall(25.0, -20.0)
		spawn_invisible_wall(22.0, -20.0)
		spawn_invisible_wall(19.0, -20.0)
		spawn_invisible_wall(18.0, -20.0)
		spawn_invisible_wall(15.0, -18.0)
		spawn_invisible_wall(12.0, -18.0)
		spawn_invisible_wall(9.0, -18.0)
		spawn_invisible_wall(6.0, -18.0)
		spawn_invisible_wall(5.0, -18.0)

		-- ridge
		spawn_invisible_wall(35.0, -9.0, 0.5)
		spawn_invisible_wall(33.0, -7.0, 0.5)
		spawn_invisible_wall(33.0, -5.0, 0.5)
		spawn_invisible_wall(33.0, -3.0, 0.5)
		spawn_invisible_wall(33.0, -1.0, 0.5)
		spawn_invisible_wall(31.0, -1.0, 0.5)

		spawn_invisible_wall(27.0, -1.0, 0.5)
		spawn_invisible_wall(25.25, 1.0, 0.5)
		spawn_invisible_wall(25.25, 3.0, 0.5)
		spawn_invisible_wall(25.25, 4.0, 0.5)
		spawn_invisible_wall(23.25, 5.0, 0.5)
		spawn_invisible_wall(23.25, 7.0, 0.5)
		spawn_invisible_wall(23.25, 9.0, 0.5)
		spawn_invisible_wall(25.25, 11.0, 0.5)
		spawn_invisible_wall(27.25, 11.0, 0.5)
		spawn_invisible_wall(29.25, 11.0, 0.5)
		spawn_invisible_wall(31.25, 11.0, 0.5)
		spawn_invisible_wall(33.25, 11.0, 0.5)
		spawn_invisible_wall(35.25, 11.0, 0.5)
		spawn_invisible_wall(37.25, 11.0, 0.5)
		spawn_invisible_wall(39.0, 11.0, 0.5)
		spawn_invisible_wall(41.0, 13.0, 0.5)
		spawn_invisible_wall(43.0, 13.0, 0.5)
		spawn_invisible_wall(45.0, 13.0, 0.5)
		spawn_invisible_wall(47.0, 13.0, 0.5)
		spawn_invisible_wall(47.0, 11.0, 0.5)
		spawn_invisible_wall(45.0, 9.0, 0.5)
		spawn_invisible_wall(45.0, 7.0, 0.5)
		spawn_invisible_wall(47.0, 5.0, 0.5)
		spawn_invisible_wall(47.0, 3.0, 0.5)
		spawn_invisible_wall(45.0, 1.0, 0.5)
		spawn_invisible_wall(43.0, -1.0, 0.5)
		spawn_invisible_wall(43.0, -3.0, 0.5)
		spawn_invisible_wall(45.0, -5.0, 0.5)
		spawn_invisible_wall(45.0, -7.0, 0.5)
		spawn_invisible_wall(45.0, -9.0, 0.5)
	end
}