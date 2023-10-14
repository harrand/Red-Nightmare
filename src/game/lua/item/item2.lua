local item = rn.item:new()
local name = "Steel Chainmail"
item:set_id(2)
item:set_name(name)
item:set_texture_name("./res/images/skins/body_armour/chainmail.png")
item:set_equipment_slot_id(rn.equipment.slot.body_armour)
rn.items[name] = item