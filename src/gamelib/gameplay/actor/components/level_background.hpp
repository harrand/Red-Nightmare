template<>
struct actor_component_params<actor_component_id::level_background>
{
	int background_texid = 0;
	int foreground_texid = 0;
};

template<>
inline mount_result actor_component_mount<actor_component_id::level_background>
(
	const actor_component<actor_component_id::level_background>& component,
	std::span<quad_renderer::quad_data> quads
)
{
	auto& bg = quads[0];
	bg.texid[0] = component.data().background_texid;
	bg.tints[0] = tz::vec4::filled(1.0f);
	bg.scale = tz::vec2::filled(1.0f);
	auto& fg = quads[1];
	fg.texid[0] = component.data().foreground_texid;
	fg.tints[0] = tz::vec4::filled(1.0f);
	fg.scale = tz::vec2::filled(1.0f);
	return {.count = 2};
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::level_background>& component)
{
	ImGui::SliderInt("Background Image", &component.data().background_texid, 0, image_id::_count);
	ImGui::SliderInt("Foreground Image", &component.data().foreground_texid, 0, image_id::_count);
}
