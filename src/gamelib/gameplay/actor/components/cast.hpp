template<>
struct actor_component_params<actor_component_id::cast>
{
	spell spell;
	tz::duration cast_begin_time = tz::system_time();
	// todo: some payload object so we know what to do when the cast is over.

	bool complete() const
	{
		return tz::system_time().seconds<std::uint64_t>() > (static_cast<std::uint64_t>(this->spell.cast.cast_time_seconds) + this->cast_begin_time.seconds<std::uint64_t>());
	}
};

template<>
inline void actor_component_update<actor_component_id::cast>
(
	actor_component<actor_component_id::cast>& component,
	float dt,
	actor& actor
)
{
	TZ_PROFZONE("cast component - update", 0xffaa0077);
	if(actor.entity.has_component<actor_component_id::humanoid_skeleton>())
	{
		// humanoid, we give it the human casting effect.
		// it might not be fully initialised yet (i.e no anim component yet), so we dont do anything until it has it.
		if(actor.entity.has_component<actor_component_id::animation>() && actor.entity.has_component<actor_component_id::sprite>())
		{
			auto& anim = actor.entity.get_component<actor_component_id::animation>()->data();
			// 6 super magic number representing the 'effect' slot.
			auto& sprite = actor.entity.get_component<actor_component_id::sprite>()->data();
			sprite.textures[6].colour_tint = component.data().spell.cast.colour;
			if(!anim.animations[6].has_value() || anim.animations[6].value().size() == 0)
			{
				anim.animations[6] = animation
				{{
					.frame_textures =
					{
						image_id::race_human_cast0,
						image_id::race_human_cast1,
						image_id::race_human_cast2,
						image_id::race_human_cast3,
						image_id::race_human_cast4,
						image_id::race_human_cast5,
						image_id::race_human_cast6,
						image_id::race_human_cast7,
					},
					.fps = 7,
					.loop = true
				}};
			}
		}
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::cast>& component)
{
	auto cast_elapsed_time = (tz::system_time() - component.data().cast_begin_time).millis<std::uint64_t>();
	ImGui::Text("Casting for %.2fs%s", cast_elapsed_time / 1000.0f, component.data().complete() ? " (done)" : "");
	ImGui::ProgressBar((cast_elapsed_time / 1000.0f) / component.data().spell.cast.cast_time_seconds);
	ImGui::SliderFloat3("Cast Colour", component.data().spell.cast.colour.data().data(), 0.0f, 1.0f);
}
