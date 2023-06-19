enum class combat_behaviour_flag
{
	chase_nearest_enemy,
	sometimes_cast_fireball,
	heal_when_low,
};
using combat_behaviour_flags = tz::enum_field<combat_behaviour_flag>;

constexpr float fireball_cd_seconds = 2.0f;
template<>
struct actor_component_params<actor_component_id::combat_behaviour>
{
	combat_behaviour_flags flags = {};
	float impl_fireball_cd = fireball_cd_seconds;
};

template<>
inline void actor_component_update<actor_component_id::combat_behaviour>
(
	actor_component<actor_component_id::combat_behaviour>& component,
	float dt,
	actor& actor_me
)
{
	TZ_PROFZONE("combat behaviour component - update", 0xffaa0077);
	auto stop_movement = [&actor_me]()
	{
		if(actor_me.actions.has_component<action_id::move_to>())
		{
			actor_me.actions.remove_component<action_id::move_to>();
		}
		if(actor_me.actions.has_component<action_id::move_to_actor>())
		{
			actor_me.actions.remove_component<action_id::move_to_actor>();
		}
		if(actor_me.actions.has_component<action_id::move_to_target>())
		{
			actor_me.actions.remove_component<action_id::move_to_target>();
		}
		if(actor_me.entity.has_component<actor_component_id::motion>())
		{
			actor_me.entity.get_component<actor_component_id::motion>()->data().direction = 0;
		}
	};
	// heal if low
	if(actor_me.casting())
	{
		return;
	}
	if(component.data().flags.contains(combat_behaviour_flag::heal_when_low)
	&& actor_me.entity.has_component<actor_component_id::damageable>()
	&& actor_me.entity.get_component<actor_component_id::damageable>()->data().hp_pct() < 0.5f)
	{
		stop_movement();
		actor_me.actions.add_component<action_id::cast>
		({
			.spell = spell_id::heal
		});
	}
	else
	{
		// target an enemy.
		actor_me.actions.add_component<action_id::target_actor_if>
		({
			.predicate = [](const actor& me, const actor& other)
			{
				return (other.entity.has_component<actor_component_id::damageable>() && !other.entity.get_component<actor_component_id::damageable>()->data().dead())
						&& get_allegience(me.faction, other.faction) == allegience_t::enemy;
			}
		});

		// do combat stuff.
		if(component.data().flags.contains(combat_behaviour_flag::chase_nearest_enemy))
		{
			actor_me.actions.add_component<action_id::move_to_target>();
		}
		component.data().impl_fireball_cd -= (dt / 1000.0f);
		if(component.data().flags.contains(combat_behaviour_flag::sometimes_cast_fireball)
		&& component.data().impl_fireball_cd <= 0.0f)
		{
			component.data().impl_fireball_cd = fireball_cd_seconds;
			stop_movement();
			actor_me.actions.add_component<action_id::cast>
			({
				.spell = spell_id::fireball
			});
		}
	}
}
