local id = 1
local levelstr = "blackrock_dungeon"
rn.level.type[levelstr] = id

local spawn_loot_chest = function(x, y)
	local ent = rn.scene():get(rn.scene():add(12))
	ent:get_element():set_position(x, y)
	local data = rn.entity_get_data(ent)
	
	local itemset = {}
	for k in pairs(rn.items) do
		table.insert(itemset, k)
	end

	local item_name = itemset[math.random(#itemset)]
	data.loot = item_name
end

local spawn_invisible_wall = function(x, y, sc)
	sc = sc or 1.0
	local ent = rn.scene():get(rn.scene():add(9))	
	ent:get_element():set_position(x, y)
	ent:get_element():set_uniform_scale(2.0 * sc)
	ent:get_element():object_set_visibility(2, false)
	local bstats = ent:get_base_stats()
	bstats:set_maximum_health(999999)
	bstats:set_defence_rating(999999)
	ent:set_base_stats(bstats)
	ent:set_health(ent:get_stats():get_maximum_health())
	rn.entity_data_write(ent, "impl.targetable", false, "impl.projectile_skip", true)
end

rn.level_handler[id] =
{
	on_load = function()
		for i=-63,63,12 do
			local y = math.random(0, 126) - 63
			spawn_loot_chest(i, y)
		end
	end
}