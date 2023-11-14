local item = rn.item:new()
local name = "Fiery Hauberk"
item:set_id(4)
item:set_name(name)
item:set_texture_name("./res/images/skins/body_armour/chainmail.png")
item:set_texture_tint(0.6, 0.15, 0.2)
item:set_equipment_slot_id(rn.equipment.slot.body_armour)
local eqb = rn.new_buff()
eqb:set_increased_spell_power(40)
eqb:set_increased_defence_rating(200)
eqb:set_increased_attack_power(50)
item:set_buff(eqb)
rn.items[name] = item