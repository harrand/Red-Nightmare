local item = rn.item:new()
local name = "Lightning Crown"
item:set_id(4)
item:set_name(name)
item:set_texture_name("./res/images/skins/helm/headband.png")
item:set_texture_tint(1.0, 0.65, 0.02)
item:set_equipment_slot_id(rn.equipment.slot.helm)
local eqb = rn.new_buff()
eqb:set_increased_movement_speed(40)
eqb:set_amplified_spell_power(1.4)
item:set_buff(eqb)
rn.items[name] = item