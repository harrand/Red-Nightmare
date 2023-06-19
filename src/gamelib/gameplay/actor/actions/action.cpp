#include "gamelib/gameplay/actor/actions/action.hpp"
#include "gamelib/gameplay/actor/system.hpp"
#include "tz/core/debug.hpp"
#include <random>
#include <algorithm>

namespace rnlib
{
	void action_entity::update()
	{
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			tz::assert(comp_ptr != nullptr);
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}

	void action_entity::dbgui()
	{
		ImGui::Text("Current Count: %zu", this->components.size());
	}

	float calc_rotation_from_dir(tz::vec2 dir)
	{
		float mul = 1.0f;
		float offset = 3.14159f;
		if(dir[0] > 0.0f)
		{
			offset = 0.0f;
			mul *= -1.0f;
		}
		return offset + (mul * std::asin(dir[1] / dir.length()));
	}

	#define ACTION_IMPL_BEGIN(T) template<> void action_invoke<T>(actor_system& system, actor& caster, action<T>& action, update_context context){
	#define ACTION_IMPL_END(T) } template void action_invoke<T>(actor_system& system, actor& caster, action<T>&, update_context);

	std::default_random_engine rng;

	ACTION_IMPL_BEGIN(action_id::teleport)
		caster.transform.position = action.data().location;
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::teleport)

	ACTION_IMPL_BEGIN(action_id::random_teleport)
		std::uniform_real_distribution dstx{context.view_bounds.first[0], context.view_bounds.second[0]};
		std::uniform_real_distribution dsty{context.view_bounds.first[1], context.view_bounds.second[1]};
		caster.actions.set_component<action_id::teleport>({.location = {dstx(rng), dsty(rng)}});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::random_teleport)

	ACTION_IMPL_BEGIN(action_id::despawn)
		system.remove(caster.uuid);
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::despawn)

	ACTION_IMPL_BEGIN(action_id::timed_despawn)
		auto seconds_elapsed = (tz::system_time() - action.data().impl_start).seconds<std::uint64_t>();
		if(seconds_elapsed > action.data().seconds_until_despawn)
		{
			caster.actions.set_component<action_id::despawn>();
			action.set_is_complete(true);
		}
	ACTION_IMPL_END(action_id::timed_despawn)

	ACTION_IMPL_BEGIN(action_id::spawn)
		actor& a = system.add(action.data().type);
		a.parent = caster.parent;
		a.transform.position = caster.transform.position;
		if(action.data().inherit_faction)
		{
			a.faction = caster.faction;
		}
		action.data().actions.transfer_components(a.actions);
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::spawn)

	ACTION_IMPL_BEGIN(action_id::cast)
		if(caster.entity.has_component<actor_component_id::damageable>())
		{
			// you can't cast while you're dead.
			if(caster.entity.get_component<actor_component_id::damageable>()->data().dead())
			{
				action.set_is_complete(true);
				return;
			}
		}
		caster.entity.add_component<actor_component_id::cast>({.spell = rnlib::create_spell(action.data().spell)});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::cast)

	ACTION_IMPL_BEGIN(action_id::cancel_cast)
		if(caster.entity.has_component<actor_component_id::cast>())
		{
			if(!caster.entity.get_component<actor_component_id::cast>()->data().complete())
			{
				caster.entity.remove_component<actor_component_id::cast>();
			}
		}
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::cancel_cast)

	ACTION_IMPL_BEGIN(action_id::move_to)
		if(caster.dead())
		{
			action.set_is_complete(true);
			return;
		}
		if(caster.entity.has_component<actor_component_id::motion>())
		{
			// set motion direction towards the target.
			auto& motion = caster.entity.get_component<actor_component_id::motion>()->data();
			motion.impl_held = true;
			motion.direction = 0;
			tz::vec2 now = system.get_global_transform(caster.uuid).position;
			tz::vec2 target = action.data().location;
			// leeway is the distance we travel in 16.66ms
			const float leeway = motion.speed * 0.1f;
			const float xdiff = now[0] - target[0];
			bool destination = false;
			if(action.data().rotate_towards)
			{
				tz::vec2 dir = target - now;
				caster.transform.rotation = calc_rotation_from_dir(dir);
			}
			if(xdiff < -leeway)
			{
				motion.direction |= move_direction::right;
			}
			else if(xdiff > leeway)
			{
				motion.direction |= move_direction::left;
			}
			else
			{
				destination = true;
			}
			const float ydiff = now[1] - target[1];
			if(ydiff < -leeway)
			{
				motion.direction |= move_direction::up;
			}
			else if(ydiff > leeway)
			{
				motion.direction |= move_direction::down;
			}
			else
			{
				if(destination)
				{
					// made it within leeway on both x and y. we're there.
					action.set_is_complete(true);
					motion.impl_held = false;
				}
			}
		}
		else
		{
			// actor can't move, we either ignore the request or teleport.
			// teleport probs makes more sense?
			tz::report("action `move_to` detected on actor (%s) without a motion component. the actor will instantly teleport to the location.", caster.name);
			caster.actions.set_component<action_id::teleport>({.location = action.data().location});
			action.set_is_complete(true);
		}
	ACTION_IMPL_END(action_id::move_to)

	ACTION_IMPL_BEGIN(action_id::move_to_actor)
		if(system.find(action.data().uuid) != nullptr)
		{
			caster.actions.set_component<action_id::move_to>
			({
				.location = system.get_global_transform(action.data().uuid).position,
				.rotate_towards = action.data().rotate_towards
			});
		}
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::move_to_actor)

	ACTION_IMPL_BEGIN(action_id::move_to_target)
		if(caster.target == actor::nullid || caster.dead())
		{
			action.set_is_complete(true);
			return;
		}
		caster.actions.set_component<action_id::move_to_actor>
		({
	   		.uuid = caster.target,
	   		.rotate_towards = action.data().rotate_towards
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::move_to_target)

	ACTION_IMPL_BEGIN(action_id::target_actor_if)
		for(const actor& a : system.container())
		{
			if(action.data().predicate(caster, a))
			{
				caster.target = a.uuid;
				return;
			}
		}
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::target_actor_if)

	ACTION_IMPL_BEGIN(action_id::move_in_direction)
		if(!caster.entity.has_component<actor_component_id::motion>())
		{
			action.set_is_complete(true);
			return;
		}
		auto& motion = caster.entity.get_component<actor_component_id::motion>()->data();
		if(!motion.impl_held)
		{
			caster.transform.position += action.data().dir.normalised() * motion.speed * (motion.impl_dt / 1000.0f);
			if(action.data().rotate_towards)
			{
				caster.transform.rotation = calc_rotation_from_dir(action.data().dir);
			}
		}
	ACTION_IMPL_END(action_id::move_in_direction)

	ACTION_IMPL_BEGIN(action_id::emit_combat_text)
		actor& text = system.add(actor_type::unknown);
		text.transform = caster.transform;
		tz::vec2 abscale = text.transform.scale;
		abscale[0] = std::abs(abscale[0]);
		abscale[1] = std::abs(abscale[1]);
		text.transform.position += abscale;
		text.transform.scale *= 2.0f;
		text.actions.add_component<action_id::timed_despawn>
		({
			.seconds_until_despawn = 1.5f
		});
		text.entity.add_component<actor_component_id::motion>
		({
			.direction = move_direction::up,
	   		.tilt_factor = 0.1f,
	   		.impl_held = true,
		});
		tz::vec3 col = tz::vec3::filled(1.0f);
		switch(action.data().type)
		{
			case combat_text_type::damage:
				col = {1.0f, 0.0f, 0.0f};
			break;
			case combat_text_type::heal:
				col = {0.0f, 1.0f, 0.0f};
			break;
		}
		text.entity.set_component<actor_component_id::label>
		({
			.text = std::to_string(action.data().amount),
			.colour = col
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::emit_combat_text)

	ACTION_IMPL_BEGIN(action_id::delayed_custom)
		auto seconds_elapsed = (tz::system_time() - action.data().impl_start).seconds<std::uint64_t>();
		if(seconds_elapsed > action.data().seconds_till_action)
		{
			caster.actions.set_component<action_id::custom>
			({
				.run = action.data().run
			});
			action.set_is_complete(true);
		}
	ACTION_IMPL_END(action_id::delayed_custom)

	ACTION_IMPL_BEGIN(action_id::custom)
		action.data().run(caster, system, context);
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::custom)
}
