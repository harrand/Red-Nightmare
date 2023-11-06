-- responsible for spawning enemies, collectables etc in the world
rn.director = {}
rn.director.credit = 0
rn.director.second_timeout = 0
rn.director.spawn_cooldown = 0
rn.director.paused = false

rn.fixed_update = function()
	tracy.ZoneBegin()
	rn.director.advance()
	tracy.ZoneEnd()
end

rn.director.advance = function()
	rn.director.second_timeout = rn.director.second_timeout + rn.fixed_delta_time	
	if rn.director.second_timeout >= 1.0 then
		rn.director.second_timeout = 0
		rn.director.on_second_pass()
	end
end

rn.director.on_second_pass = function()
	-- director gains 1 credit per second
	if rn.director.paused then return end
	rn.director.credit = rn.director.credit + 2
	rn.director.spawn_cooldown = rn.director.spawn_cooldown - 1

	if rn.director.spawn_cooldown <= 0 then
		-- cost 5: zombie
		if rn.director.credit > 5 then
			local credit_cost = 5
			rn.director.credit = rn.director.credit - credit_cost
			rn.scene():add(2)
			rn.director.spawn_cooldown = 5
		end

		-- cost 10: armoured zombie
		if rn.director.credit > 10 then
			local credit_cost = 10
			rn.director.credit = rn.director.credit - credit_cost
			local zomb = rn.scene():get(rn.scene():add(2))
			rn.equip(zomb, "Steel Chainmail")
			rn.equip(zomb, "Iron Sallet")
			rn.director.spawn_cooldown = 15
		end

		-- cost 200: OMEGA zombie
		if rn.director.credit > 100 then
			local credit_cost = 100
			rn.director.credit = rn.director.credit - credit_cost
			local zomb = rn.scene():get(rn.scene():add(2))
			rn.equip(zomb, "Steel Chainmail")
			rn.equip(zomb, "White Cloth Headband")

			local omegabuff = rn.new_buff()
			omegabuff:set_increased_health(1500)
			omegabuff:set_increased_movement_speed(100)
			omegabuff:set_amplified_attack_power(2.0)
			zomb:apply_buff(omegabuff)
			zomb:get_element():set_uniform_scale(zomb:get_element():get_uniform_scale() * 1.3)
			rn.director.spawn_cooldown = 25
		end
	end
	print("director credits: " .. rn.director.credit)
end