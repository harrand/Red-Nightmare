using move_direction_t = int;
namespace move_direction
{
	enum actor_motion_e
	{
		right = 0b0001, left = 0b0010, up = 0b0100, down = 0b1000
	};
}

template<>
struct actor_component_params<actor_component_id::motion>
{
	move_direction_t direction = 0;
	float speed = 1.0f;
	float tilt_timer = 0.0f;
	float tilt_factor = 0.02f;
	float tilt_rate = 0.01f;
};

template<>
inline void actor_component_update<actor_component_id::motion>
(
	actor_component<actor_component_id::motion>& component,
	float dt,
	actor& actor
)
{
	float corrected_speed = component.data().speed / std::max(std::popcount(static_cast<unsigned int>(component.data().direction)), 1);
	if(component.data().direction & move_direction::right)
	{
		actor.transform.local_position[0] += corrected_speed * (dt / 1000.0f);
	}
	if(component.data().direction & move_direction::left)
	{
		actor.transform.local_position[0] -= corrected_speed * (dt / 1000.0f);
	}
	if(component.data().direction & move_direction::up)
	{
		actor.transform.local_position[1] += corrected_speed * (dt / 1000.0f);
	}
	if(component.data().direction & move_direction::down)
	{
		actor.transform.local_position[1] -= corrected_speed * (dt / 1000.0f);
	}
	if(component.data().direction == 0)
	{
		component.data().tilt_timer = 0.0f;
	}
	else
	{
		component.data().tilt_timer += dt;
	}
	component.data().direction = 0;
	actor.transform.local_rotation = std::sin(component.data().tilt_timer * component.data().tilt_rate * component.data().speed) * component.data().tilt_factor * std::clamp(component.data().speed, 1.0f, 2.0f);
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::motion>& component)
{
	ImGui::SliderFloat("Speed", &component.data().speed, 0.0f, 3.0f);
	ImGui::SliderFloat("Tilt Factor", &component.data().tilt_factor, 0.0f, 0.5f);
	ImGui::SliderFloat("Tilt Rate", &component.data().tilt_rate, 0.0f, 0.25f);
	ImGui::Text("Direction:  %s", [](move_direction_t dir)
	{
		std::string str = "";
		if(dir & move_direction::right)
		{
			str += 'r';
		}
		if(dir & move_direction::left)
		{
			str += 'l';
		}
		if(dir & move_direction::up)
		{
			str += 'u';
		}
		if(dir & move_direction::down)
		{
			str += 'd';
		}
		if(str.empty()) str = "none";
		return str;
	}(component.data().direction).c_str());
	if(ImGui::Button("Defaults"))
	{
		component.data() = {};
	}
}
