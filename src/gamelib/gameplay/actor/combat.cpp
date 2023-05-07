#include "gamelib/gameplay/actor/combat.hpp"
#include "gamelib/gameplay/actor/combat_log.hpp"
#include "gamelib/gameplay/actor/actions/action.hpp"
#include "gamelib/gameplay/actor/entity.hpp"
#include "gamelib/gameplay/actor/spell/spell.hpp"

namespace rnlib::combat
{
	struct combat_data_t
	{
		combat_log log;
	} combat_data;

	void damage(actor& target, actor* attacker, spell_id cause, std::size_t amt)
	{
		std::optional<std::size_t> overkill = std::nullopt;
		tz::assert(target.entity.has_component<actor_component_id::damageable>());
		auto& damageable = target.entity.get_component<actor_component_id::damageable>()->data();
		// process damage calculation.
		if(damageable.health <= amt)
		{
			overkill = amt - damageable.health;
			amt = damageable.health;
			// this will kill the actor.
		}
		damageable.health -= amt;

		target.actions.add_component<action_id::emit_combat_text>
		({
			.type = combat_text_type::damage,
	   		.amount = amt
		});

		combat_data.log.add
		({
			.spell = cause,
	   		.caster_uuid = attacker->uuid,
	   		.target_uuid = target.uuid,
	   		.type = combat_text_type::damage,
	   		.value = amt,
	   		.over = overkill
		});
	}

	void heal(actor& target, actor* healer, spell_id cause, std::size_t amt)
	{
		tz::assert(target.entity.has_component<actor_component_id::damageable>());
		std::optional<std::size_t> overheal = std::nullopt;
		auto& damageable = target.entity.get_component<actor_component_id::damageable>()->data();
		if(damageable.health + amt >= damageable.max_health)
		{
			// overheal.
			overheal = amt - (damageable.max_health - damageable.health);
			amt = (damageable.max_health - damageable.health);
		}
		damageable.health += amt;

		target.actions.add_component<action_id::emit_combat_text>
		({
			.type = combat_text_type::heal,
	   		.amount = amt
		});
		target.actions.add_component<action_id::spawn>
		({
			.type = actor_type::heal_effect
		});

		combat_data.log.add
		({
			.spell = cause,
	   		.caster_uuid = healer->uuid,
	   		.target_uuid = target.uuid,
	   		.type = combat_text_type::heal,
	   		.value = amt,
	   		.over = overheal
		});
	}

	const combat_log& get_log()
	{
		return combat_data.log;
	}
}
