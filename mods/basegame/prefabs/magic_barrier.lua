rn.mods.basegame.prefabs.magic_barrier = 
{
	description = "Absorb shield is displayed on the entity.",
	static_init = function()
		rn.renderer():add_texture("sprite.magic_barrier", "basegame/res/sprites/magic_barrier.png")
	end,
	pre_instantiate = function(uuid)
		return rn.entity.prefabs.sprite.pre_instantiate(uuid)
	end,
	instantiate = function(uuid)
		rn.entity.prefabs.sprite.instantiate(uuid)
		rn.entity.prefabs.sprite.set_texture(uuid, "sprite.magic_barrier")
		rn.entity.prefabs.light_emitter.instantiate(uuid)
		rn.entity.prefabs.sprite.set_rotation(uuid, -1.5708)
end,
	update = function(uuid, delta_seconds)
		local sc = rn.current_scene()
		local t = sc:entity_read(uuid, "timer") or 0.0
		t = t + (delta_seconds * 8)
		sc:entity_write(uuid, "timer", t)

		local scale = 2.0 + (0.1 * math.sin(t))
		rn.entity.prefabs.sprite.set_scale(uuid, scale)

		local magic_type = sc:entity_read(uuid, "magic_type")
		if magic_type ~= nil then
			local colour = rn.spell.schools[magic_type].colour
			rn.entity.prefabs.sprite.set_colour(uuid, colour[1], colour[2], colour[3])
			rn.entity.prefabs.light_emitter.set_power(uuid, scale * 0.75)
			rn.entity.prefabs.light_emitter.set_colour(uuid, colour[1], colour[2], colour[3])
		end

		rn.entity.prefabs.light_emitter.update(uuid, delta_seconds)
		rn.entity.prefabs.sticky.update(uuid, delta_seconds)
		-- note: by default we dont set a despawn timer here, for that reason this will never be set off.
		-- however, external forces may want us to delete ourselves after a delay, so we give them the option by carrying it out here if its set elsewhere.
	end,
	on_remove = rn.mods.basegame.prefabs.light_emitter.on_remove,
}