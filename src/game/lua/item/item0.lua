local item = rn.item:new()
local name = "Iron Sallet"
item:set_id(0)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/med_helm.png")
item:set_texture_tint(0.3, 0.3, 0.3)
item:set_equipment_slot_id(rn.equipment.slot.helm)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(10)
item:set_buff(eqb)
rn.items[name] = item