template<>
struct actor_component_params<actor_component_id::collide>
{
	float mass_ratio = 0.01f;
	std::function<bool(const actor& self, const actor& other)> collide_if = [](const auto& a, const auto& b){(void)a;(void)b; return true;};
	bool debug_draw = false;
	tz::vec2 aabb_offset = tz::vec2::zero();
	tz::vec2 aabb_scale = tz::vec2::filled(1.0f);
	tz::vec3 debug_draw_colour = tz::vec3::filled(1.0f);

	box get_bounding_box(const actor& owner) const
	{
		box ret = owner.transform.get_bounding_box();
		tz::vec2 centre = ret.get_centre();
		tz::vec2 extents = ret.get_dimensions();
		centre += this->aabb_offset;
		extents[0] *= this->aabb_scale[0];
		extents[1] *= this->aabb_scale[1];
		return {centre - (extents * 0.5f), centre + (extents * 0.5f)};
	}
};

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
		quad.pos = component.data().aabb_offset;
		quad.scale = component.data().aabb_scale;
		quad.texid[0] = image_id::border32;
		quad.tints[0] = component.data().debug_draw_colour.with_more(1.0f);
		quad.layer = 50;
		smallquad.pos = component.data().aabb_offset;
		smallquad.scale = component.data().aabb_scale * 0.05f;
		smallquad.texid[0] = image_id::border8;
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
	ImGui::SliderFloat2("Offset", component.data().aabb_offset.data().data(), -1.0f, 1.0f);
	ImGui::SliderFloat2("Scale", component.data().aabb_scale.data().data(), 0.0f, 1.0f);
}
