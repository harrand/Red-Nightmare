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

	const char* get_damage_type_name(const combat_damage_types& type)
	{
		// base types.
		if(type == combat_damage_types{combat_damage_type::untyped})
		{
			return "";
		}
		if(type == combat_damage_types{combat_damage_type::physical})
		{
			return "Physical";
		}
		if(type == combat_damage_types{combat_damage_type::fire})
		{
			return "Fire";
		}
		if(type == combat_damage_types{combat_damage_type::frost})
		{
			return "Frost";
		}
		if(type == combat_damage_types{combat_damage_type::earth})
		{
			return "Earth";
		}
		if(type == combat_damage_types{combat_damage_type::air})
		{
			return "Air";
		}
		if(type == combat_damage_types{combat_damage_type::shadow})
		{
			return "Shadow";
		}
		if(type == combat_damage_types{combat_damage_type::nihimancy})
		{
			return "Nihimancy";
		}
		if(type == combat_damage_types{combat_damage_type::divine})
		{
			return "Divine";
		}

		// multiple types
		if(type == combat_damage_types{combat_damage_type::shadow, combat_damage_type::fire})
		{
			return "Shadowflame";
		}
		if(type == combat_damage_types{combat_damage_type::divine, combat_damage_type::fire})
		{
			return "Radiant";
		}
		return "Mystery";
	}

	void damage(actor& target, actor* attacker, spell_id cause, std::size_t amt, const combat_damage_types& type)
	{
		std::optional<std::size_t> overkill = std::nullopt;
		tz::assert(target.entity.has_component<actor_component_id::damageable>());
		auto& damageable = target.entity.get_component<actor_component_id::damageable>()->data();
		// process damage calculation.
		bool killed = false;
		if(damageable.health <= amt)
		{
			overkill = amt - damageable.health;
			amt = damageable.health;
			// this will kill the actor. register it as a new kill if they are currently alive.
			if(damageable.health > 0)
			{
				killed = true;
			}
		}
		damageable.health -= amt;

		target.actions.add_component<action_id::emit_combat_text>
		({
			.type = combat_text_type::damage,
	   		.amount = amt
		});

		combat_event evt
		{
			.spell = cause,
	   		.caster_uuid = attacker->uuid,
	   		.target_uuid = target.uuid,
	   		.type = combat_text_type::damage,
	   		.value = amt,
	   		.damage_type = type,
	   		.over = overkill
		};
		combat_data.log.add(evt);
		if(killed && target.entity.has_component<actor_component_id::action_listener>())
		{
			auto& action_listener = target.entity.get_component<actor_component_id::action_listener>()->data();
			for(auto& callback : action_listener.on_death)
			{
				callback(target, evt);
			}
		}
	}

	void heal(actor& target, actor* healer, spell_id cause, std::size_t amt, const combat_damage_types& type)
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
	   		.damage_type = type,
	   		.over = overheal
		});
	}

	const combat_log& get_log()
	{
		return combat_data.log;
	}
}
