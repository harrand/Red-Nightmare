template<>
struct actor_component_params<actor_component_id::keyboard_control>
{
	std::unordered_map<move_direction_t, tz::wsi::key> movement_keys =
	{
		{move_direction::right, tz::wsi::key::d},
		{move_direction::left, tz::wsi::key::a},
		{move_direction::up, tz::wsi::key::w},
		{move_direction::down, tz::wsi::key::s}
	};
	bool enabled = true;
};

template<>
inline void actor_component_update<actor_component_id::keyboard_control>
(
	actor_component<actor_component_id::keyboard_control>& component,
	float dt,
	actor& actor
)
{
	TZ_PROFZONE("keyboard_control component - update", 0xffaa0077);
	if(!actor.entity.has_component<actor_component_id::motion>())
	{
		actor.entity.add_component<actor_component_id::motion>();
	}
	auto& motion = actor.entity.get_component<actor_component_id::motion>()->data();
	if(motion.impl_held)
	{
		// something has taken control of our movement. ignore all inputs.
		return;
	}
	const auto& state = tz::window().get_keyboard_state();
	auto key_down = [state, &component](move_direction_t dir)
	{
		return component.data().enabled && tz::wsi::is_key_down(state, component.data().movement_keys[dir]);
	};
	if(key_down(move_direction::right))
	{
		motion.direction |= move_direction::right;
	}
	if(key_down(move_direction::left))
	{
		motion.direction |= move_direction::left;
	}
	if(key_down(move_direction::up))
	{
		motion.direction |= move_direction::up;
	}
	if(key_down(move_direction::down))
	{
		motion.direction |= move_direction::down;
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::keyboard_control>& component)
{
	ImGui::Checkbox("Enabled", &component.data().enabled);
}
