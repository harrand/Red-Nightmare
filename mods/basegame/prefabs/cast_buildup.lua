rn.mods.basegame.prefabs.cast_buildup = 
{
	description = "Sprite particle effect displayed while an entity casts a magic spell.",
	frame_count = 6,
	static_init = function()
		for i=0,rn.entity.prefabs.cast_buildup.frame_count,1 do
			rn.renderer():add_texture("sprite.cast_buildup" .. i, "basegame/res/sprites/cast_buildup/cast_buildup" .. i .. ".png")
		end
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.cast_buildup0")
		rn.entity.prefabs.sprite.set_emissive_map(uuid, "sprite.cast_buildup0")
		rn.entity.prefabs.light_emitter.instantiate(uuid)
	end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "timer") or 0.0
		local time_warp = sc:entity_read(uuid, "time_warp") or 1.0
		t = t + (delta_seconds * time_warp)
		local frame_id = math.floor((t * 10.0) % rn.entity.prefabs.cast_buildup.frame_count)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.cast_buildup" .. frame_id)
		rn.entity.prefabs.sprite.set_emissive_map(uuid, "sprite.cast_buildup" .. frame_id)
		sc:entity_write(uuid, "timer", t)

		local magic_type = sc:entity_read(uuid, "magic_type")
		local power_override = sc:entity_read(uuid, "power_override")
		if magic_type ~= nil then
			local colour = rn.spell.schools[magic_type].colour
			rn.entity.prefabs.sprite.set_colour(uuid, colour[1], colour[2], colour[3])
			rn.entity.prefabs.sprite.set_emissive_tint(uuid, colour[1], colour[2], colour[3])
			rn.entity.prefabs.light_emitter.set_power(uuid, power_override or (t * 0.2))
			rn.entity.prefabs.light_emitter.set_colour(uuid, colour[1], colour[2], colour[3])
		end

		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
		rn.entity.prefabs.sticky.update(uuid, delta_seconds)
		-- note: by default we dont set a despawn timer here, for that reason this will never be set off.
		-- however, external forces may want us to delete ourselves after a delay, so we give them the option by carrying it out here if its set elsewhere.
	end,
	on_remove = rn.mods.basegame.prefabs.light_emitter.on_remove,
}