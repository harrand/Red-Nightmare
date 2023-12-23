local item = rn.item:new()
local name = "White Legion Platebody"
item:set_id(6)
item:set_name(name)
item:set_texture_name("./res/images/skins/body_armour/chainmail.png")
item:set_texture_tint(0.9, 0.8, 0.9)
item:set_equipment_slot_id(rn.equipment.slot.body_armour)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(85)
eqb:set_increased_movement_speed(20)
item:set_buff(eqb)
rn.items[name] = item