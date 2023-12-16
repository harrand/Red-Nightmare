local id = 2
local typestr = "enemy_melistra_zombie"
rn.entity.type[typestr] = id

rn.entity_handler[id] =
{
	static_init = function()
		tracy.ZoneBeginN(typestr .. " - static init")
		rn.texture_manager():register_texture(typestr .. ".skin", "./res/images/skins/entity2.png")
		rn.texture_manager():register_texture(typestr .. ".helm", "./res/images/skins/helm/med_helm.png")
		rn.texture_manager():register_texture(typestr .. ".body", "./res/images/skins/body_armour/chainmail.png")
		tracy.ZoneEnd()
	end,
	preinit = function(ent)
		ent:set_name("Zombie (Melistra)")
		ent:set_model(rn.model.humanoid)
		local stats = ent:get_base_stats()
		stats:set_maximum_health(50)
		ent:set_base_stats(stats)
	end,
	postinit = function(ent)
		local texh = rn.texture_manager():get_texture(typestr .. ".skin")
		ent:get_element():object_set_texture_handle(3, 0, texh)
		local bodytexh = rn.texture_manager():get_texture(typestr .. ".body")
		ent:get_element():object_set_texture_handle(5, 0, bodytexh)
		local helmtexh = rn.texture_manager():get_texture(typestr .. ".helm")
		ent:get_element():object_set_texture_handle(7, 0, helmtexh)
		local sc = ent:get_element():get_uniform_scale()
		ent:get_element():set_uniform_scale(sc * 0.5)

		ent:set_faction(rn.faction_id.player_enemy)

		local stats = ent:get_base_stats()
		stats:set_movement_speed(1.75)
		stats:set_attack_power(10)
		ent:set_base_stats(stats)

	end,
	on_struck = function(ent, evt)
		local damager = rn.scene():get_uid(evt.damager)
		rn.entity_get_data(ent).target = damager
	end,
	update = function(ent)
		local data = rn.entity_get_data(ent)
		data.collided_this_update = false
		local target_args = {aggro_range = 20, target_relationship = "hostile"}
		local target_args2 = {aggro_range = target_args.aggro_range * 2, target_relationship = "hostile"}
		if data.target == nil then
			data.target = rn.entity_target_entity(ent, target_args)
		else
			if not rn.impl_entity_entity_valid_target(ent, target_args2, data.target) then
				data.target = nil
			end
		end
		-- attempt to attack any enemy nearby
		rn.for_each_collision(ent, function(ent2)
			if not ent:is_dead() and not data.collided_this_update and ent2:is_valid() and not ent2:is_dead() and rn.get_relationship(ent, ent2) == "hostile" and rn.entity_get_data(ent2).impl.targetable ~= false then
				data.collided_this_update = true
				rn.cast_spell({ent = ent, ability_name = "Melee", cast_type_override = rn.cast.type.melee_unarmed_lunge})
			end
		end)
		if not rn.is_casting(ent) and not ent:is_dead() then
			if data.target ~= nil then
				rn.entity_move_to_entity({ent = ent, movement_anim_name = "ZombieWalk"}, data.target)
			else
				-- otherwise just move right forever???
				-- todo: wander around aimlessly
				--rn.entity_move{ent = ent, dir = "right", movement_anim_name = "ZombieWalk"}
			end
		end
	end
}