local item = rn.item:new()
local name = "White Cloth Headband"
item:set_id(1)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/headband.png")
item:set_equipment_slot_id(rn.equipment.slot.helm)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(3)
item:set_buff(eqb)
rn.items[name] = item