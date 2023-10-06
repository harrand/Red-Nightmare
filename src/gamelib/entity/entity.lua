rn.initialise_entity = function(type)
	-- assume variable exists in global "rn_impl_new_entity"!
	tz.assert(rn_impl_new_entity ~= nil)
	print("well met!" .. rn_impl_new_entity:get_name())
end