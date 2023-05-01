template<>
struct actor_component_params<actor_component_id::label>
{
	// what should be displayed on the label? note: if an empty string, the actor name will be used instead.
	std::string text = "";
	// what colour is the text.
	tz::vec3 colour = tz::vec3::filled(1.0f);
};

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::label>& component)
{
	static char buf[64] = "";
	if(ImGui::InputText("Label", buf, 64))
	{
		component.data().text = buf;
	}
	ImGui::SameLine();
	if(ImGui::Button("Clear"))
	{
		std::memset(buf, 64, 0);
	}
	ImGui::SliderFloat3("Colour", component.data().colour.data().data(), 0.0f, 1.0f);
}
