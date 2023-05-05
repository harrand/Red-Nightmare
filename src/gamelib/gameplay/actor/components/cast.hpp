template<>
struct actor_component_params<actor_component_id::cast>
{
	spell spell;
	float cast_length_seconds = 1.0f;
	cast_direction direction = cast_direction::omni;
	tz::duration cast_begin_time = tz::system_time();
	// todo: some payload object so we know what to do when the cast is over.

	bool complete() const
	{
		return tz::system_time().seconds<std::uint64_t>() > (static_cast<std::uint64_t>(this->cast_length_seconds) + this->cast_begin_time.seconds<std::uint64_t>());
	}
};

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::cast>& component)
{
	auto cast_elapsed_time = (tz::system_time() - component.data().cast_begin_time).millis<std::uint64_t>();
	ImGui::Text("Casting for %.2fs%s", cast_elapsed_time / 1000.0f, component.data().complete() ? " (done)" : "");
	ImGui::ProgressBar((cast_elapsed_time / 1000.0f) / component.data().cast_length_seconds);
}
