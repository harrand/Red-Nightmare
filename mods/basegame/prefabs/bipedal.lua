local base_subobj = 9
local chest_subobj = 3
local helm_subobj = 5
local legs_subobj = 7

-- new model:
-- 11-12 left shoulder. 13 elbow. 14 hand
-- 15-16 right shoulder, 17 elbow. 18 hand.

local base_scale = 0.75

rn.mods.basegame.prefabs.bipedal =
{
	description = "Entity is a 3D bipedal animated humanoid",
	left_hand = 20,
	right_hand = 24,
	default_movement_speed = 6,
	static_init = function()
		rn.renderer():add_model("bipedal", "basegame/res/models/bipedal.glb")
	end,
	pre_instantiate = function(uuid)
		return "bipedal"
	end,
	instantiate = function(uuid)
		local sc = rn.current_scene()
		sc:entity_write(uuid, ".boundary_scale", 1.0)
		sc:entity_set_subobject_pixelated(uuid, base_subobj, true)
		sc:entity_set_subobject_pixelated(uuid, helm_subobj, true)
		sc:entity_set_subobject_pixelated(uuid, chest_subobj, true)
		sc:entity_set_subobject_pixelated(uuid, legs_subobj, true)
		rn.entity.prefabs.combat_stats.set_base_movement_speed(uuid, rn.entity.prefabs.bipedal.default_movement_speed)
		rn.entity.prefabs.bipedal.set_scale(uuid, 1.0, 1.0, 1.0)
		rn.entity.prefabs.bipedal.set_visible(uuid, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, helm_subobj, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, chest_subobj, true)
		--rn.entity.prefabs.bipedal.set_subobject_visible(uuid, legs_subobj, true)
		rn.entity.prefabs.bipedal.face_direction(uuid, 0.0, 1.0)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local currently_playing = sc:entity_get_playing_animation(uuid)
		local should_stop_move_animation = (currently_playing == rn.entity.prefabs.bipedal.get_run_animation(uuid)) and not moving
		if currently_playing == nil then
			sc:entity_play_animation(uuid, rn.entity.prefabs.bipedal.get_idle_animation(uuid))
		end
	end,
	on_collision = function(me, other)
		return rn.entity.prefabs.combat_stats.is_alive(me)
	end,
	on_move = function(uuid, xdiff, ydiff, zdiff)
		local sc = rn.current_scene()
		local currently_playing = sc:entity_get_playing_animation(uuid)
		local movement_anim_multiplier = 0.9 * rn.entity.prefabs.combat_stats.get_movement_speed(uuid) / rn.entity.prefabs.bipedal.default_movement_speed
		local anim = rn.entity.prefabs.bipedal.get_run_animation(uuid)
		if currently_playing ~= anim then
			rn.current_scene():entity_play_animation(uuid, anim, false, movement_anim_multiplier)
		end
		-- if we're moving both horizontally and vertically, always prefer horizontal facing.
		if math.abs(xdiff) >= math.abs(ydiff) then
			ydiff = 0
		end
		rn.entity.prefabs.bipedal.face_direction(uuid, -xdiff, -ydiff)
	end,
	on_stop_moving = function(uuid)
		if not rn.spell.is_casting(uuid) then
			rn.current_scene():entity_play_animation(uuid, rn.entity.prefabs.bipedal.get_idle_animation(uuid))
		end
	end,
	on_cast_begin = function(uuid, spellname)
		local spelldata = rn.spell.spells[spellname]
		local base_cast_time = spelldata.cast_duration

		local haste = rn.entity.prefabs.combat_stats.get_haste(uuid) or 0.0
		-- new_casting_time = base_casting_time / (1 + haste)
		local cast_duration = base_cast_time / (1.0 + haste)

		local cast_anim = nil
		local artificial_anim_delay = 0.0
		if spelldata.magic_type == "physical" or spelldata.magic_type == nil then
			cast_anim = "Melee1H_Attack"
			artificial_anim_delay = -0.125
		else
			if spelldata.two_handed == true then
				if spelldata.cast_type == "omni" then
					cast_anim = "Cast2H_Omni"
				else
					cast_anim = "Cast2H_Directed"
				end
			else
				if spelldata.cast_type == "omni" then
					cast_anim = "Cast1H_Omni"
				else
					cast_anim = "Cast1H_Directed"
				end
			end
		end
		local animation_duration = rn.current_scene():entity_get_animation_length(uuid, cast_anim)
		-- note: the end frame of the animation is unlikely to be when we want the cast to go off.
		-- for now, let's say we want the anim to be 65% done when the cast actually goes off.
		animation_duration = animation_duration * (0.65 + artificial_anim_delay)
		rn.entity.prefabs.bipedal.play_animation(uuid, cast_anim, false, animation_duration / cast_duration)
	end,
	on_death = function(uuid, dmg, magic_type, enemy_uuid)
		rn.current_scene():entity_play_animation(uuid, "ZombieDeath")
		rn.item.drop_all_equipment(uuid)
	end,
	on_equip = function(uuid, itemname)
		local sc = rn.current_scene()
		local itemdata = rn.item.items[itemname]
		local subobject = rn.entity.prefabs.bipedal.item_slot_to_subobject(itemdata.slot)
		if (itemdata.slot ~= rn.item.slot.left_hand) and (itemdata.slot ~= rn.item.slot.right_hand) then
			-- armour
			rn.entity.prefabs.bipedal.set_subobject_visible(uuid, subobject, true)
			local col = itemdata.colour or {1.0, 1.0, 1.0}
			rn.entity.prefabs.bipedal.set_subobject_colour(uuid, subobject, col[1], col[2], col[3])
			rn.entity.prefabs.bipedal.set_subobject_texture(uuid, subobject, itemdata.texture)
		else
			-- weapon
			local prefab_name = itemdata.weapon_prefab

			tz.assert(prefab_name ~= nil, "no weapon_prefab specified by item " .. itemname)

			local weapon_entity = sc:add_entity(prefab_name)
			rn.entity.prefabs.sprite.set_colour(weapon_entity, itemdata.colour[1], itemdata.colour[2], itemdata.colour[3])
			rn.entity.prefabs.sticky.stick_to_subobject(weapon_entity, uuid, subobject, true, false)
			sc:entity_write(uuid, "weapon_entity" .. itemdata.slot, weapon_entity)
		end
	end,
	on_unequip = function(uuid, itemname)
		local sc = rn.current_scene()
		local itemdata = rn.item.items[itemname]
		local subobject = rn.entity.prefabs.bipedal.item_slot_to_subobject(itemdata.slot)
		if (itemdata.slot ~= rn.item.slot.left_hand) and (itemdata.slot ~= rn.item.slot.right_hand) then
			-- armour
			rn.entity.prefabs.bipedal.set_subobject_visible(uuid, subobject, false)
		else
			-- weapon
			local weapon_entity = sc:entity_read(uuid, "weapon_entity" .. itemdata.slot)
			tz.assert(weapon_entity ~= nil, "no weapon entity?")
			sc:entity_write(uuid, "weapon_entity" .. itemdata.slot, nil)
			sc:remove_entity(weapon_entity)
		end
	end,
	item_slot_to_subobject = function(itemslot)
		if itemslot == rn.item.slot.none then
			return nil
		elseif itemslot == rn.item.slot.helm then
			return helm_subobj
		elseif itemslot == rn.item.slot.chest then
			return chest_subobj
		elseif itemslot == rn.item.slot.legs then
			return legs_subobj
		elseif itemslot == rn.item.slot.left_hand then
			return rn.mods.basegame.prefabs.bipedal.left_hand
		elseif itemslot == rn.item.slot.right_hand then
			return rn.mods.basegame.prefabs.bipedal.right_hand
		else
			tz.assert(false, "Unknown item slot " .. itemslot)
		end
	end,
	play_animation = function(uuid, animation_name, loop, time_warp)
		rn.current_scene():entity_play_animation(uuid, animation_name, loop, time_warp)
	end,

	set_subobject_visible = function(uuid, subobject, visible)
		rn.current_scene():entity_set_subobject_visible(uuid, subobject, visible)
	end,
	set_subobject_texture = function(uuid, subobject, texname)
		rn.current_scene():entity_set_subobject_texture(uuid, subobject, texname)
	end,
	set_subobject_colour = function(uuid, subobject, r, g, b)
		rn.current_scene():entity_set_subobject_colour(uuid, subobject, r, g, b)
	end,
	set_visible = function(uuid, visible)
		rn.entity.prefabs.bipedal.set_subobject_visible(uuid, base_subobj, visible)	
	end,
	set_texture = function(uuid, texname)
		rn.entity.prefabs.bipedal.set_subobject_texture(uuid, base_subobj, texname)
	end,
	get_texture = function(uuid)
		return rn.current_scene():entity_get_subobject_texture(uuid, base_subobj)
	end,
	set_colour = function(uuid, r, g, b)
		rn.entity.prefabs.bipedal.set_subobject_colour(uuid, base_subobj, r, g, b)
	end,
	get_colour = function(uuid)
		return rn.current_scene():entity_get_subobject_colour(uuid, base_subobj)
	end,

	set_chest_visible = function(uuid, visible)
		rn.entity.prefabs.bipedal.set_subobject_visible(uuid, chest_subobj, visible)
	end,
	set_chest_texture = function(uuid, texname)
		rn.entity.prefabs.bipedal.set_subobject_texture(uuid, chest_subobj, texname)
	end,
	set_chest_colour = function(uuid, r, g, b)
		rn.entity.prefabs.bipedal.set_subobject_colour(uuid, chest_subobj, r, g, b)
	end,
	set_helm_visible = function(uuid, visible)
		rn.entity.prefabs.bipedal.set_subobject_visible(uuid, helm_subobj, visible)
	end,
	set_helm_texture = function(uuid, texname)
		rn.entity.prefabs.bipedal.set_subobject_texture(uuid, helm_subobj, texname)
	end,
	set_helm_colour = function(uuid, r, g, b)
		rn.entity.prefabs.bipedal.set_subobject_colour(uuid, helm_subobj, r, g, b)
	end,
	set_legs_visible = function(uuid, visible)
		rn.entity.prefabs.bipedal.set_subobject_visible(uuid, legs_subobj, visible)
	end,
	set_legs_texture = function(uuid, texname)
		rn.entity.prefabs.bipedal.set_subobject_texture(uuid, legs_subobj, texname)
	end,
	set_legs_colour = function(uuid, r, g, b)
		rn.entity.prefabs.bipedal.set_subobject_colour(uuid, legs_subobj, r, g, b)
	end,

	set_position = function(uuid, x, y)
		rn.entity.prefabs.sprite.set_position(uuid, x, y)
	end,
	get_position = function(uuid)
		return rn.entity.prefabs.sprite.get_position(uuid)
	end,
	set_rotation = function(uuid, ry, rx, rz)
		-- euler to quaternion
		local sin = math.sin
		local cos = math.cos
		local qx = sin(rz/2.0) * cos(rx/2.0) * cos(ry/2.0) - cos(rz/2.0) * sin(rx/2.0) * sin(ry/2.0)
		local qy = cos(rz/2.0) * sin(rx/2.0) * cos(ry/2.0) + sin(rz/2.0) * cos(rx/2.0) * sin(ry/2.0)
		local qz = cos(rz/2.0) * cos(rx/2.0) * sin(ry/2.0) - sin(rz/2.0) * sin(rx/2.0) * cos(ry/2.0)
		local qw = cos(rz/2.0) * cos(rx/2.0) * cos(ry/2.0) + sin(rz/2.0) * sin(rx/2.0) * sin(ry/2.0)
		rn.current_scene():entity_set_local_rotation(uuid, qx, qy, qz, qw)
	end,
	get_rotation = function(uuid)
		local x, y, z, w = rn.current_scene():entity_get_local_rotation(uuid)
		local t0 = 2.0 * (w * x + y * z)
		local t1 = 1.0 - 2.0 * (x * x + y * y)
		local roll = math.atan(t0, t1)
		local t2 = 2.0 * (w * y - z * x)
		if t2 > 1.0 then t2 = 1.0 end
		if t2 < -1.0 then t2 = -1.0 end
		local pitch = math.asin(t2)
		local t3 = 2.0 * (w * z + x * y)
		local t4 = 1.0 - 2.0 * (y * y + z * z)
		local yaw = math.atan(t3, t4)
		return yaw, pitch, roll
	end,
	get_face_direction = function(uuid)
		local dx = rn.current_scene():entity_read(uuid, "facedirx") or 0.0
		local dy = rn.current_scene():entity_read(uuid, "facediry") or 0.0
		return dx, dy
	end,
	face_direction = function(uuid, dx, dy)
		-- face in a directional vector. useful for calculating casting directions.
		rn.current_scene():entity_write(uuid, "facedirx", dx)
		rn.current_scene():entity_write(uuid, "facediry", dy)
		local tilt_factor = 0.125
		if math.abs(dx) > math.abs(dy) then
			local signx = math.abs(dx)/dx
			-- if face right, rx and rz is positive
			-- if face left, rx is negative and rz is positive
			rn.entity.prefabs.bipedal.set_rotation(uuid, 0.0, -signx * math.pi / 2.0, 0.0)
		else
			local signy = math.abs(dy)/dy
			rn.entity.prefabs.bipedal.set_rotation(uuid, 0.0, math.max(-signy, 0.0) * math.pi, math.pi * signy * tilt_factor)
		end
	end,
	set_scale = function(uuid, sx, sy, sz)
		rn.current_scene():entity_set_local_scale(uuid, sx * base_scale, sy * base_scale, sz * base_scale)
	end,
	get_scale = function(uuid)
		local x, y, z = rn.current_scene():entity_get_local_scale(uuid)
		x = x * base_scale
		y = y * base_scale
		z = z * base_scale
		return x, y, z
	end,
	get_can_equip = function(uuid)
		return rn.current_scene():entity_read(uuid, "can_equip")
	end,
	set_can_equip = function(uuid, can_equip)
		rn.current_scene():entity_write(uuid, "can_equip", can_equip)
	end,
	set_run_animation = function(uuid, run_anim)
		rn.current_scene():entity_write(uuid, "run_animation", run_anim)
	end,
	get_run_animation = function(uuid)
		local run_override = rn.current_scene():entity_read(uuid, "run_animation")
		if run_override ~= nil then return run_override end
		if rn.item.get_weapon_class_equipped_slot(uuid, "torch") then
			return "TorchRun"
		elseif rn.item.get_weapon_class_equipped_slot(uuid, "shield") then
			return "Melee1H_Run"
		else
			return "Run"
		end
	end,
	set_idle_animation = function(uuid, idle_anim)
		rn.current_scene():entity_write(uuid, "idle_animation", idle_anim)
	end,
	get_idle_animation = function(uuid)
		local idle_override = rn.current_scene():entity_read(uuid, "idle_animation")
		if idle_override ~= nil then return idle_override end
		if rn.item.get_weapon_class_equipped_slot(uuid, "torch") then
			return "TorchIdle"
		elseif rn.item.get_weapon_class_equipped_slot(uuid, "shield") then
			return "Melee1H_Idle_Examine"
		else
			return "Idle"
		end
	end,
	set_death_animation = function(uuid, death_anim)
		rn.current_scene():entity_write(uuid, "death_animation", death_anim)
	end,
	get_death_animation = function(uuid)
		local death_override = rn.current_scene():entity_read(uuid, "death_animation")
		if death_override ~= nil then return death_override end
		if rn.item.get_weapon_class_equipped_slot(uuid, "torch") then
			return "CastDeath"
		elseif rn.item.get_weapon_class_equipped_slot(uuid, "shield") then
			return "Melee1H_Death"
		else
			return "Melee2H_Death"
		end
	end,
}