-- responsible for spawning enemies, collectables etc in the world
rn.director = {}
-- how many credits does the director currently have?
rn.director.credit = 0
-- impl detail. used to call on_second_pass every second
rn.director.second_timeout = 0
-- initial cooldown (in seconds) before enemies start spawning
rn.director.spawn_cooldown = 10
-- whether paused or not. if true, director won't spawn stuff nor gain credits.
rn.director.paused = false
-- impl detail. used to increase spawn credit_rate over time. don't touch.
rn.director.credit_rate_counter = 0
-- multiplier for a threshold value the credit_rate counter must reach before the director gets more aggressive.
-- higher means lower difficulty. 0 is an invalid value.
rn.director.credit_rate_increase_threshold = 120
-- how many credits should the director obtain every second? scales with spawn credit_rate.
rn.director.credit_rate = 1
-- controls general credit_rate of increase of spawn credit_rate. higher difficulty means spawn credit_rate increases more often.
rn.director.difficulty = 1

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
	rn.director.credit = rn.director.credit + rn.director.credit_rate
	rn.director.credit_rate_counter = rn.director.credit_rate_counter + 1

	-- every 30 seconds, credit_rate increases
	if rn.director.credit_rate_counter >= 120 / rn.director.difficulty then
		rn.director.credit_rate = rn.director.credit_rate + 1
		rn.director.credit_rate_counter = 0
	end
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

		-- cost 50: banshee
		if rn.director.credit > 50 then
			local credit_cost = 50
			rn.director.credit = rn.director.credit - credit_cost
			rn.scene():add(11)
			rn.director.spawn_cooldown = 15
		end

		-- cost 100: horde of rushers
		if rn.director.credit > 100 then
			local credit_cost = 100
			rn.director.credit = rn.director.credit - credit_cost
			for i=0,3,1 do
				local zomb = rn.scene():get(rn.scene():add(2))
				rn.equip(zomb, "White Legion Helmet")
				local speedy = rn.new_buff()
				speedy:set_increased_movement_speed(150)
				zomb:apply_buff(speedy)
				zomb:get_element():set_uniform_scale(zomb:get_element():get_uniform_scale() * 0.7)
			end
			rn.director.spawn_cooldown = 7
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
			zomb:set_health(zomb:get_stats():get_maximum_health())
			zomb:get_element():set_uniform_scale(zomb:get_element():get_uniform_scale() * 1.3)
			rn.director.spawn_cooldown = 25
		end
	end
end