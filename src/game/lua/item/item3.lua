local item = rn.item:new()
local name = "White Legion Helmet"
item:set_id(3)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/med_helm.png")
item:set_texture_tint(2.0, 2.0, 2.0)
item:set_equipment_slot_id(rn.equipment.slot.helm)
rn.items[name] = item