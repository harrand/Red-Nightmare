rn.mods.basegame.prefabs.magic_ball_base =
{
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
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magicball0")
		rn.current_scene():entity_write(uuid, ".boundary_scale", 0.5)
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
	end,
	on_collision = function(uuid_a, uuid_b)
		-- just destroy whatever it touches, no matter what.
		-- todo: don't do this. fire/frost/shadow bolts should have unified collision behaviour, but not this.
		rn.current_scene():remove_entity(uuid_a)
		rn.current_scene():remove_entity(uuid_b)
		return false
	end
}

-- for each magic school (that isnt physical), create a new prefab based on magic_ball_base.
-- e.g firebolt, frostbolt etc...
for schoolname, schooldata in pairs(rn.spell.schools) do
	if schoolname ~= "physical" then
		-- is a magic school.
		rn.mods.basegame.prefabs[schoolname .. "bolt"] =
		{
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