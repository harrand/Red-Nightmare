local item = rn.item:new()
local name = "Steel Chainmail"
item:set_id(2)
item:set_name(name)
item:set_texture_name("./res/images/skins/body_armour/chainmail.png")
item:set_equipment_slot_id(rn.equipment.slot.body_armour)
local eqb = rn.new_buff()
eqb:set_increased_defence_rating(30)
item:set_buff(eqb)
rn.items[name] = item