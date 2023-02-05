enum class move_direction
{
	right, left, up, down
};

template<>
struct actor_component_params<actor_component_id::keyboard_control>
{
	std::array<tz::wsi::key, 4> movement_keys =
	{
		tz::wsi::key::d, // d = right
		tz::wsi::key::a, // a = left
		tz::wsi::key::w, // w = up
		tz::wsi::key::s // s = down
	};
};

template<>
inline void actor_component_update<actor_component_id::keyboard_control>
(
	actor_component<actor_component_id::keyboard_control>& component,
	float dt,
	actor& actor
)
{
	const auto& state = tz::window().get_keyboard_state();
	auto key_down = [state, &component](move_direction dir)
	{
		return tz::wsi::is_key_down(state, component.data().movement_keys[static_cast<int>(dir)]);
	};
	if(key_down(move_direction::right))
	{
		actor.transform.local_position[0] += 2.0f * (dt / 1000.0f);
	}
	if(key_down(move_direction::left))
	{
		actor.transform.local_position[0] -= 2.0f * (dt / 1000.0f);
	}
	if(key_down(move_direction::up))
	{
		actor.transform.local_position[1] += 2.0f * (dt / 1000.0f);
	}
	if(key_down(move_direction::down))
	{
		actor.transform.local_position[1] -= 2.0f * (dt / 1000.0f);
	}
}
