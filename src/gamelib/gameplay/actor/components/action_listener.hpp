using action_event_callback_t = void(actor& a, actor& p1);

template<>
struct actor_component_params<actor_component_id::action_listener>
{
	std::vector<std::function<action_event_callback_t>> on_collide = {};
};
