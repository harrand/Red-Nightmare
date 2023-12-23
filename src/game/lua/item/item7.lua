local item = rn.item:new()
local name = "White Legion Platelegs"
item:set_id(7)
item:set_name(name)
item:set_texture_name("./res/images/skins/legs/chain.png")
item:set_texture_tint(0.9, 0.8, 0.9)
item:set_equipment_slot_id(rn.equipment.slot.legs)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(70)
eqb:set_increased_movement_speed(20)
item:set_buff(eqb)
rn.items[name] = item