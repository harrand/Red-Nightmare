template<>
struct actor_component_params<actor_component_id::collide>
{
	float mass_ratio = 0.01f;
};

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::collide>& component)
{
	ImGui::SliderFloat("Mass Ratio", &component.data().mass_ratio, 0.0f, 1.0f);
}
