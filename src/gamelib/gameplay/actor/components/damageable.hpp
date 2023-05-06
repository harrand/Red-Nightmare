template<>
struct actor_component_params<actor_component_id::damageable>
{
	int health = 1;
	int max_health = 1;
	bool invincible = false;
	bool dead() const
	{
		return !this->invincible && this->health <= 0;
	}
};

template<>
inline void actor_component_update<actor_component_id::damageable>
(
	actor_component<actor_component_id::damageable>& component,
	float dt,
	actor& actor
)
{
	TZ_PROFZONE("damageable component - update", 0xffaa0077);
	if(actor.entity.has_component<actor_component_id::motion>())
	{
		auto& motion = actor.entity.get_component<actor_component_id::motion>()->data();
		motion.impl_held = component.data().dead();
		if(component.data().dead())
		{
			motion.direction = 0;
		}
	}
}

template<>
inline void actor_component_dbgui(actor_component<actor_component_id::damageable>& component)
{
	if(ImGui::InputInt("Max Health", &component.data().max_health))
	{
		component.data().health = std::min(component.data().health, component.data().max_health);
	}
	ImGui::SliderInt("Health", &component.data().health, 0, component.data().max_health);
	ImGui::Checkbox("Invincible", &component.data().invincible);
	ImGui::Text("Dead: %s", component.data().dead() ? "true" : "false");
}
