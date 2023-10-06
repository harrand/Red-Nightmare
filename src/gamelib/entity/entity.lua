rn.model =
{
	quad = 0,
	humanoid = 1
}

-- called early on in initialisation - before the scene element is attached
-- this is your opportunity to choose which model the entity will use.
-- note: as the scene element doesn't exist, you cannot use `ent:get_element()`
-- at all here.
rn.entity_preinit = function(type)
	-- assume variable exists in global "rn_impl_new_entity"!
	tz.assert(rn_impl_new_entity ~= nil)
	ent = rn_impl_new_entity
	ent:set_name("DOCTOR MICHAEL MORBIUS");
	ent:set_model(rn.model.humanoid);
end

-- called straight after the entity has finished initialisation. you can
-- do pretty much whatever you want at this point.
rn.entity_postinit = function(type)
	tz.assert(rn_impl_new_entity ~= nil)
	ent = rn_impl_new_entity
	ent:get_element():play_animation(1, false)
end