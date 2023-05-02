template<>
struct actor_component_params<actor_component_id::collide>
{
	float mass_ratio = 0.01f;
	bool debug_draw = false;
	tz::vec3 debug_draw_colour = tz::vec3::filled(1.0f);
};

template<>
inline void actor_component_update<actor_component_id::collide>
(
	actor_component<actor_component_id::collide>& component,
	float dt,
	actor& actor
)
{
	box b = actor.transform.get_bounding_box();
}

template<>
inline mount_result actor_component_mount<actor_component_id::collide>
(
	const actor_component<actor_component_id::collide>& component,
	std::span<quad_renderer::quad_data> quads
)
{
	if(component.data().debug_draw)
	{
		auto& quad = quads[0];
		auto& smallquad = quads[1];
		quad.texid[0] = image_id::border32;
		quad.scale = tz::vec2::filled(1.0f);
		quad.tints[0] = component.data().debug_draw_colour.with_more(1.0f);
		quad.layer = 50;
		smallquad.texid[0] = image_id::border8;
		smallquad.scale = tz::vec2::filled(0.05f);
		smallquad.tints[0] = component.data().debug_draw_colour.with_more(1.0f);
		smallquad.layer = -1;
		return {.count = 2};
	}
	return {.count = 0};
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::collide>& component)
{
	ImGui::SliderFloat("Mass Ratio", &component.data().mass_ratio, 0.0f, 1.0f);
	ImGui::Checkbox("Debug Draw Collider", &component.data().debug_draw);
	ImGui::SliderFloat3("Collider Colour", component.data().debug_draw_colour.data().data(), 0.0f, 1.0f);
}
