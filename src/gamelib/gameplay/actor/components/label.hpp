template<>
struct actor_component_params<actor_component_id::label>
{
	// what should be displayed on the label? note: if an empty string, the actor name will be used instead.
	std::string text = "";
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
}
