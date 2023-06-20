using action_collide_event_callback_t = void(actor& a, actor& p1);
using action_click_event_callback_t = void(actor& a, tz::wsi::mouse_button button);
using action_death_event_callback_t = void(actor& a, combat_event killing_event);

template<>
struct actor_component_params<actor_component_id::action_listener>
{
	std::vector<std::function<action_collide_event_callback_t>> on_collide = {};
	std::vector<std::function<action_click_event_callback_t>> on_click = {};
	std::vector<std::function<action_death_event_callback_t>> on_death = {};
};
