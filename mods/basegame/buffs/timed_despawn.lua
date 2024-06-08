rn.mods.basegame.buffs.timed_despawn =
{
	name = "Despawn after a certain period of time",
	internal = true,
	on_remove = function(uuid)
		rn.current_scene():remove_entity(uuid)
	end,
}