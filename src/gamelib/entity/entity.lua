rn.model =
{
	quad = 0,
	humanoid = 1
}

rn.initialise_entity = function(type)
	-- assume variable exists in global "rn_impl_new_entity"!
	tz.assert(rn_impl_new_entity ~= nil)
	print("well met!" .. rn_impl_new_entity:get_name())
	rn_impl_new_entity:set_name("DOCTOR MICHAEL MORBIUS");
	rn_impl_new_entity:set_model(rn.model.humanoid);
end