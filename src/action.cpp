#include "action.hpp"
#include "scene.hpp"
#include "hdk/profile.hpp"

namespace game
{
	#define ACTION_IMPL_BEGIN(T) template<> void action_invoke<T>(SceneData& scene, Action<T>& action){
	#define ACTION_IMPL_END(T) } template void action_invoke<T>(SceneData& scene, Action<T>&);

//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::GotoMouse)
		scene.actor().entity.add<ActionID::GotoTarget>
		({
			.target_position = scene.mouse_position
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::GotoMouse)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::GotoPlayer)
		auto players = scene.get_living_players();
		if(!players.empty())
		{
			scene.actor().entity.add<ActionID::GotoActor>
			({
				.actor_id = players.front()
			});
			action.set_is_complete(true);
		}
	ACTION_IMPL_END(ActionID::GotoPlayer)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::GotoActor)
		scene.actor().entity.set<ActionID::GotoTarget>
		({
			.target_position = scene.get_quad(action.data().actor_id).position,
			.timeout = 1000.0f
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::GotoActor)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::GotoTarget)
		if(scene.actor().dead())
		{
			return;
		}
		if(action.data().timeout <= 0.0f)
		{
			action.set_is_complete(true);
			return;
		}
		// Get time passed since last update.
		unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - scene.actor().last_update.millis<unsigned long long>();
		action.data().timeout -= delta_millis;

		scene.chase_target = action.data().target_position;
		// If they're within touching distance, this is done.
		float dist = (action.data().target_position - scene.quad().position).length();
		if(dist <= scene.touch_distance * 2)
		{
			action.set_is_complete(true);
		}
	ACTION_IMPL_END(ActionID::GotoTarget)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::LaunchToMouse)
		const hdk::vec2 to_mouse = scene.mouse_position - scene.quad().position;
		scene.actor().entity.add<ActionID::Launch>
		({
			.direction = scene.mouse_position - scene.quad().position,
			.speed_multiplier = action.data().speed_multiplier
		});
	ACTION_IMPL_END(ActionID::LaunchToMouse)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::LaunchToPlayer)
		auto players = scene.get_living_players();
		if(players.empty())
		{
			return;
		}
		hdk::vec2 target_pos = scene.get_quad(players.front()).position;
		hdk::vec2 to_target = target_pos - scene.quad().position;
		scene.actor().entity.add<ActionID::Launch>
		({
			.direction = to_target,
			.speed_multiplier = action.data().speed_multiplier
		});
	ACTION_IMPL_END(ActionID::LaunchToPlayer)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Launch)
		if(scene.actor().dead())
		{
			return;
		}
		const float speed = scene.actor().get_current_stats().movement_speed * action.data().speed_multiplier;
		unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - scene.actor().last_update.millis<unsigned long long>();
		scene.quad().position += action.data().direction.normalised() * speed * static_cast<float>(delta_millis) / 5.0f;
	ACTION_IMPL_END(ActionID::Launch)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::TeleportToPlayer)
		auto players = scene.get_living_players();
		if(players.empty())
		{
			return;
		}
		scene.actor().entity.add<ActionID::Teleport>
		({
			.position = scene.get_quad(players.front()).position
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::TeleportToPlayer)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::RandomTeleport)
		auto bound_pair = scene.get_world_boundaries();
		std::uniform_real_distribution<float> distx{bound_pair.first[0], bound_pair.second[0]};
		std::uniform_real_distribution<float> disty{bound_pair.first[1], bound_pair.second[1]};
		scene.actor().entity.add<ActionID::Teleport>
		({
			.position = {distx(scene.rng), disty(scene.rng)}
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::RandomTeleport)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Teleport)
		scene.quad().position = action.data().position;
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::Teleport)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::HorizontalFlip)
		scene.quad().scale[0] = -std::abs(scene.quad().scale[0]);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::HorizontalFlip)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::VerticalFlip)
		scene.quad().scale[1] = -std::abs(scene.quad().scale[1]);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::VerticalFlip)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::SpawnActor)
		std::size_t id = scene.spawn_actor(action.data().actor);
		if(action.data().inherit_faction)
		{
			scene.get_actor(id).faction = scene.actor().faction;
		}
		scene.get_quad(id).position = scene.quad().position;
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::SpawnActor)
//--------------------------------------------------------------------------------------------------

	void ActionEntity::update()
	{
		HDK_PROFZONE("ActionEntity - Update", 0xFF00AA00);
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}
}
