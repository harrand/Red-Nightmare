function foo()
	tz.report("foo!")
end

rn.entity.prefabs["morbius"] = 
{
	pre_instantiate = function(uuid)
		print("i'm preparing to morb...")
		return "humanoid"
	end,
	instantiate = function(uuid)
		print("it's morbin' time!")
	end,
	update = function(uuid)
		print("MORB")
	end
}