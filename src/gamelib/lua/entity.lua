rn = rn or {}
rn.entity = rn.entity or {}

rn.entity.preinit = function(uuid)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text("Entity Preinit - " .. tostring(uuid))
	obj:set_name("Lua Entity Preinit")

	tz.report("preinit " .. tostring(uuid))
end

rn.entity.init = function(uuid)
	local obj <close> = tz.profzone_obj:new()
	obj:set_text("Entity Init - " .. tostring(uuid))
	obj:set_name("Lua Entity Preinit")

	tz.report("init " .. tostring(uuid))
end
