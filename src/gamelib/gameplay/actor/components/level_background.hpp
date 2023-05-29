template<>
struct actor_component_params<actor_component_id::level_background>
{
	int background_texid = image_id::undefined;
	int foreground_texid = image_id::invisible;
};

template<>
inline mount_result actor_component_mount<actor_component_id::level_background>
(
	const actor_component<actor_component_id::level_background>& component,
	std::span<quad_renderer::quad_data> quads
)
{
	std::size_t count = 0;
	if(component.data().background_texid != image_id::invisible)
	{
		auto& bg = quads[count++];
		bg.texid[0] = component.data().background_texid;
		bg.tints[0] = tz::vec4::filled(1.0f);
		bg.scale = tz::vec2::filled(1.0f);
		bg.layer = 1;
	}
	if(component.data().foreground_texid != image_id::invisible)
	{
		auto& fg = quads[count++];
		fg.texid[0] = component.data().foreground_texid;
		fg.tints[0] = tz::vec4::filled(1.0f);
		fg.scale = tz::vec2::filled(1.0f);
	}
	return {.count = count};
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::level_background>& component)
{
	ImGui::SliderInt("Background Image", &component.data().background_texid, 0, image_id::_count - 1);
	ImGui::SliderInt("Foreground Image", &component.data().foreground_texid, 0, image_id::_count - 1);
}
