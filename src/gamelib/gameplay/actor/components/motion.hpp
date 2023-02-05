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
	float speed;
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
	component.data().direction = 0;
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::motion>& component)
{
	ImGui::SliderFloat("Speed", &component.data().speed, 0.0f, 1.0f);
}
