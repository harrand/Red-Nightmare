rn.mods.basegame.prefabs.magic_ball_base =
{
	description = "Meta-Entity representing a bolt of unknown magic type.",
	frame_count = 3,
	static_init = function()
		for i=0,rn.entity.prefabs.magic_ball_base.frame_count,1 do
			rn.renderer():add_texture("sprite.magicball" .. i, "basegame/res/sprites/magic_ball/magic_ball" .. i .. ".png")
		end
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball0")
		local sc = rn.current_scene()
		sc:entity_write(uuid, ".boundary_scale", 0.5)
		sc:entity_write(uuid, ".movement_speed", 0.25)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "timer") or 0.0
		t = t + delta_seconds
		local frame_id = math.floor((t * 10.0) % rn.entity.prefabs.magic_ball_base.frame_count)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball" .. frame_id)
		sc:entity_write(uuid, "timer", t)


		rn.entity.prefabs.mouse_controlled.update(uuid, delta_seconds)

		local tarx = sc:entity_read(uuid, "target_location_x")
		local tary = sc:entity_read(uuid, "target_location_y")
		if tarx ~= nil and tary ~= nil then
			rn.entity.prefabs.sprite.lookat(uuid, tarx, tary, math.pi / -2.0)
		end
		rn.entity.prefabs.timed_despawn.update(uuid, delta_seconds)
	end,
	on_collision = function(uuid_a, uuid_b)
		-- todo: don't do this is the other thing is "friendly"
		-- despawn ourselves, then deal damage to the collidee with dmg equal to our max health.
		local dmg = rn.entity.prefabs.combat_stats.get_max_hp(uuid_a)
		local magic_type = rn.current_scene():entity_read(uuid_a, "magic_type")
		local owner_id = rn.current_scene():entity_read(uuid_a, "owner")
		if magic_type == nil then
			magic_type = "physical"
		end
		if owner_id == uuid_b then
			-- collided with whomsoever casted me. don't do anything.
			return false
		end
		rn.entity.prefabs.combat_stats.dmg(uuid_b, dmg, magic_type, uuid_a)
		rn.current_scene():remove_entity(uuid_a)
		return false
	end,
	set_damage = function(uuid, dmg)
		-- damage dealt is equal to max hp.
		rn.entity.prefabs.combat_stats.set_base_max_hp(uuid, dmg)
	end,
	get_damage = function(uuid)
		return rn.entity.prefabs.combat_stats.get_base_max_hp(uuid)
	end,
	set_target = function(uuid, tarx, tary)
		local sc = rn.current_scene()
		sc:entity_write(uuid, "target_location_x", tarx)
		sc:entity_write(uuid, "target_location_y", tary)
	end
}

-- for each magic school (that isnt physical), create a new prefab based on magic_ball_base.
-- e.g firebolt, frostbolt etc...
for schoolname, schooldata in pairs(rn.spell.schools) do
	if schoolname ~= "physical" then
		-- is a magic school.
		rn.mods.basegame.prefabs[schoolname .. "bolt"] =
		{
			description = "Represents a magical bolt of " .. schoolname,
			pre_instantiate = rn.mods.basegame.prefabs.magic_ball_base.pre_instantiate,
			instantiate = function(uuid)
				rn.mods.basegame.prefabs.magic_ball_base.instantiate(uuid)
				rn.current_scene():entity_write(uuid, "magic_type", schoolname)

				-- todo: saner magic colours
				local school = rn.spell.schools[schoolname]
				rn.entity.prefabs.sprite.set_colour(uuid, school.colour[1], school.colour[2], school.colour[3])
			end,
			update = rn.mods.basegame.prefabs.magic_ball_base.update,
			on_collision = rn.mods.basegame.prefabs.magic_ball_base.on_collision
		}
	end
end