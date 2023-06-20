enum class combat_behaviour_flag
{
	chase_nearest_enemy,
	sometimes_cast_fireball,
	heal_when_low,
	wander_if_idle,
};
using combat_behaviour_flags = tz::enum_field<combat_behaviour_flag>;

struct combat_spell
{
	spell_id spellid;
	float cooldown_seconds = 0.0f;
	float impl_current_cd = 0.0f;
};

constexpr float fireball_cd_seconds = 2.0f;
template<>
struct actor_component_params<actor_component_id::combat_behaviour>
{
	combat_behaviour_flags flags = {};
	std::vector<combat_spell> aggressive_ranged_spells = {};
	std::vector<combat_spell> aggressive_melee_spells = {};
	float defensive_hp_ratio_cutoff = 0.5f;
	std::vector<combat_spell> defensive_ranged_spells = {};
	std::vector<combat_spell> defensive_melee_spells = {};

	combat_spell* get_aggressive_spell()
	{
		auto iter = std::find_if(this->aggressive_ranged_spells.begin(), this->aggressive_ranged_spells.end(),
		[](const combat_spell& sp)
		{
			return sp.impl_current_cd <= 0.0f;
		});
		if(iter == this->aggressive_ranged_spells.end())
		{
			return nullptr;
		}
		return &*iter;
	}

	combat_spell* get_defensive_spell()
	{
		auto iter = std::find_if(this->defensive_ranged_spells.begin(), this->defensive_ranged_spells.end(),
		[](const combat_spell& sp)
		{
			return sp.impl_current_cd <= 0.0f;
		});
		if(iter == this->defensive_ranged_spells.end())
		{
			return nullptr;
		}
		return &*iter;
	}
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
	for(combat_spell& cs : component.data().aggressive_ranged_spells)
	{
		cs.impl_current_cd -= (dt / 1000.0f);
	}
	for(combat_spell& cs : component.data().aggressive_melee_spells)
	{
		cs.impl_current_cd -= (dt / 1000.0f);
	}
	for(combat_spell& cs : component.data().defensive_ranged_spells)
	{
		cs.impl_current_cd -= (dt / 1000.0f);
	}
	for(combat_spell& cs : component.data().defensive_melee_spells)
	{
		cs.impl_current_cd -= (dt / 1000.0f);
	}
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
	combat_spell* def_spell = component.data().get_defensive_spell();
	if(component.data().flags.contains(combat_behaviour_flag::heal_when_low)
	&& actor_me.entity.has_component<actor_component_id::damageable>()
	&& actor_me.entity.get_component<actor_component_id::damageable>()->data().hp_pct() < 0.5f
	&& component.data().defensive_ranged_spells.size()
	&& def_spell != nullptr)
	{
		stop_movement();
		// find a defensive on cooldown.
		actor_me.actions.add_component<action_id::cast>
		({
			.spell = def_spell->spellid
		});
		def_spell->impl_current_cd = def_spell->cooldown_seconds;
	}
	else
	{
		// target an enemy.
		actor_me.actions.add_component<action_id::target_actor_if>
		({
			.predicate = [](const actor& me, const actor& other)
			{
				return !other.dead()
				&& get_allegience(me.faction, other.faction) == allegience_t::enemy;
			}
		});

		// do combat stuff.
		if(component.data().flags.contains(combat_behaviour_flag::chase_nearest_enemy))
		{
			actor_me.actions.add_component<action_id::move_to_target>();
		}
		combat_spell* aggro_spell = component.data().get_aggressive_spell();
		if(component.data().flags.contains(combat_behaviour_flag::sometimes_cast_fireball)
		&& aggro_spell != nullptr)
		{
			stop_movement();
			actor_me.actions.add_component<action_id::cast>
			({
				.spell = aggro_spell->spellid
			});
			aggro_spell->impl_current_cd = aggro_spell->cooldown_seconds;
		}
		else if(component.data().flags.contains(combat_behaviour_flag::wander_if_idle))
		{
			tz::vec2 random_location = actor_me.transform.position;
			auto sr = std::random_device{};
			std::uniform_real_distribution dst{-0.5f, 0.5f};
			random_location[0] += dst(sr);
			random_location[1] += dst(sr);
			actor_me.actions.add_component<action_id::move_to>
			({
				.location = random_location
			});
		}
	}
}
