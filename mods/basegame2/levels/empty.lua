rn.mods.basegame2.levels.empty =
{
	on_load = function()
		local p = rn.current_scene():add_entity("player")
		rn.level.data_write("player", p)
	end
}