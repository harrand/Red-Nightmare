local item = rn.item:new()
local name = "White Legion Helmet"
item:set_id(3)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/med_helm.png")
item:set_texture_tint(0.0, 0.5, 0.0)
item:set_equipment_slot_id(rn.equipment.slot.helm)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(25)
eqb:set_increased_movement_speed(20)
item:set_buff(eqb)
rn.items[name] = item