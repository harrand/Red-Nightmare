local id = 0
local levelstr = "blanchfield"
rn.level.type[levelstr] = id

local spawn_tree = function(x, y)
	local ent = rn.scene():get(rn.scene():add(9))	
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

rn.level_handler[id] =
{
	on_load = function()
		-- spawn a bunch of trees :0
		for i=0,10,1 do
			spawn_tree(i * 3, i * 3)
		end
	end
}