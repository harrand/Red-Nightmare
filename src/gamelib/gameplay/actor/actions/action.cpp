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

	#define ACTION_IMPL_BEGIN(T) template<> void action_invoke<T>(actor_system& system, actor& caster, action<T>& action, update_context context){
	#define ACTION_IMPL_END(T) } template void action_invoke<T>(actor_system& system, actor& caster, action<T>&, update_context);

	std::default_random_engine rng;

	ACTION_IMPL_BEGIN(action_id::teleport)
		caster.transform.local_position = action.data().location;
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

	ACTION_IMPL_BEGIN(action_id::spawn)
		actor& a = system.add(action.data().type);
		a.transform = caster.transform;
		action.data().actions.transfer_components(a.actions);
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::spawn)

	ACTION_IMPL_BEGIN(action_id::cast)
		caster.entity.add_component<actor_component_id::cast>({.spell = rnlib::create_spell(action.data().spell)});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::cast)

	ACTION_IMPL_BEGIN(action_id::move_to)
		if(caster.entity.has_component<actor_component_id::motion>())
		{
			// set motion direction towards the target.
			auto& motion = caster.entity.get_component<actor_component_id::motion>()->data();
			motion.impl_held = true;
			motion.direction = 0;
			tz::vec2 now = caster.transform.get_position();
			tz::vec2 target = action.data().location;
			// leeway is the distance we travel in 16.66ms
			const float leeway = motion.speed * 0.1f;
			const float xdiff = now[0] - target[0];
			bool destination = false;
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
}
