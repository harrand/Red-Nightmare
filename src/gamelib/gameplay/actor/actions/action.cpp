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

	#define ACTION_IMPL_BEGIN(T) template<> void action_invoke<T>(actor_system& system, actor& caster, action<T>& action){
	#define ACTION_IMPL_END(T) } template void action_invoke<T>(actor_system& system, actor& caster, action<T>&);

	ACTION_IMPL_BEGIN(action_id::teleport)
		caster.transform.local_position = action.data().location;
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::teleport)

	ACTION_IMPL_BEGIN(action_id::random_teleport)
		std::default_random_engine rng;
		std::uniform_real_distribution dist{0.0f, 1.0f};
		caster.actions.set_component<action_id::teleport>({.location = {dist(rng), dist(rng)}});
		action.set_is_complete(true);
	ACTION_IMPL_END(action_id::random_teleport)
}