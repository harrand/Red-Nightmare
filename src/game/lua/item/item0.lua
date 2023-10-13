local item = rn.item:new()
local name = "Iron Sallet"
item:set_id(0)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/med_helm.png")
item:set_equipment_slot_id(rn.equipment.slot.helm)
rn.items[name] = item