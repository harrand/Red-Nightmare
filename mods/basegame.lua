local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs =
	{
		morbius =
		{
			pre_instantiate = function(uuid)
				print("i'm preparing to morb...")
				return "plane"
			end,
			instantiate = function(uuid)
				print("it's morbin' time!")
			end,
			update = function(uuid)

			end
		}
	},
}