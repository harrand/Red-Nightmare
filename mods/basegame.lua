function foo()
	tz.report("foo!")
end

local mod = "basegame"

rn.mods[mod] =
{
	description = "Base game content for Red Nightmare",
	prefabs = {}
}

rn.mods[mod].prefabs["morbius"] = 
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