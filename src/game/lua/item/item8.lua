local item = rn.item:new()
local name = "Iron Legplates"
item:set_id(8)
item:set_name(name)
item:set_texture_name("./res/images/skins/legs/chain.png")
item:set_texture_tint(0.3, 0.3, 0.3)
item:set_equipment_slot_id(rn.equipment.slot.legs)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(15)
eqb:set_increased_movement_speed(5)
item:set_buff(eqb)
rn.items[name] = item