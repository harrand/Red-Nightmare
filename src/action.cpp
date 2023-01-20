#include "action.hpp"
#include "scene.hpp"
#include "tz/core/profile.hpp"
#include "tz/wsi/keyboard.hpp"

namespace game
{

//--------------------------------------------------------------------------------------------------

	void ActionEntity::update()
	{
		TZ_PROFZONE("ActionEntity - Update", 0xFF00AA00);
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}

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
	ACTION_IMPL_BEGIN(ActionID::MoveRelative)
		scene.actor().entity.set<ActionID::GotoTarget>
		({
			.target_position = scene.quad().position + action.data().displacement,
			.timeout = action.data().timeout
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::MoveRelative)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::LaunchToMouse)
		const tz::vec2 to_mouse = scene.mouse_position - scene.quad().position;
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
		tz::vec2 target_pos = scene.get_quad(players.front()).position;
		tz::vec2 to_target = target_pos - scene.quad().position;
		scene.actor().entity.add<ActionID::Launch>
		({
			.direction = to_target,
			.speed_multiplier = action.data().speed_multiplier
		});
	ACTION_IMPL_END(ActionID::LaunchToPlayer)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::LaunchRandomDirection)
		std::uniform_real_distribution<float> dist{-1.0f, 1.0f};
		scene.actor().entity.add<ActionID::Launch>
		({
			.direction = tz::vec2{dist(scene.rng), dist(scene.rng)},
			.speed_multiplier = action.data().speed_multiplier
		});
	ACTION_IMPL_END(ActionID::LaunchRandomDirection)
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
		action.data().actions.copy_components(scene.get_actor(id).entity);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::SpawnActor)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Respawn)
		scene.actor().respawn();
		// Note: Action is no longer valid, because respawn clears them all!
	ACTION_IMPL_END(ActionID::Respawn)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::RespawnAs)
		scene.actor().type = action.data().actor;
		scene.actor().respawn();
		// Note: Action is no longer valid, because respawn clears them all!
	ACTION_IMPL_END(ActionID::RespawnAs)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Despawn)
		scene.actor().entity.add<ActionID::Die>();
		if(scene.actor().flags.has<FlagID::DoNotGarbageCollect>())
		{
			scene.actor().flags.remove<FlagID::DoNotGarbageCollect>();
		}
		scene.actor().flags.add<FlagID::CustomGarbageCollectPeriod>({.delay_millis = 0ull});
	ACTION_IMPL_END(ActionID::Despawn)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Die)
		scene.actor().base_stats.current_health = 0;
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::Die)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::ApplyBuff)
		scene.actor().buffs.add(action.data().buff);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::ApplyBuff)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::ApplyBuffToActor)
		scene.get_actor(action.data().actor_id).buffs.add(action.data().buff);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::ApplyBuffToActor)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::ApplyBuffToTarget)
		if(scene.actor().target == Actor::NullID)
		{
			return;
		}
		
		scene.get_actor(scene.get_actor_from_uuid(scene.actor().target)).buffs.add(action.data().buff);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::ApplyBuffToTarget)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::ApplyBuffToPlayers)
		auto players = scene.get_living_players();
		for(auto p : players)
		{
			scene.get_actor(p).buffs.add(action.data().buff);
		}
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::ApplyBuffToPlayers)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::DelayedAction)
		if(action.data().delay_millis <= 0.0f)
		{
			action.data().actions.copy_components(scene.actor().entity);
			action.set_is_complete(true);
			return;
		}
		unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - scene.actor().last_update.millis<unsigned long long>();
		action.data().delay_millis -= delta_millis;
	ACTION_IMPL_END(ActionID::DelayedAction)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::Cast)
		bool should_cancel = false;
		if(action.data().player_cancel_cast_escape && scene.actor().flags.has<FlagID::Player>() && tz::wsi::is_key_down(tz::window().get_keyboard_state(), tz::wsi::key::esc))
		{
			// The player has pressed escape to cancel their cast.
			should_cancel = true;
		}
		if(!action.data().cast_while_moving && !scene.actor().motion.empty())
		{
			// We're moving and this cast does not support that, cancel it.
			should_cancel = true;
		}
		if(!action.data().cast_while_dead && scene.actor().dead())
		{
			should_cancel = true;
		}
		if(should_cancel)
		{
			action.set_is_complete(true);
			return;
		}
		if(action.data().cast_time_millis <= 0.0f)
		{
			// Cast complete, do the thing!
			action.data().actions.copy_components(scene.actor().entity);
			action.set_is_complete(true);
			return;
		}
		unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - scene.actor().last_update.millis<unsigned long long>();
		action.data().cast_time_millis -= delta_millis;
	ACTION_IMPL_END(ActionID::Cast)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::ApplyFlag)
		action.data().flags.transfer_components(scene.actor().flags);
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::ApplyFlag)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::NextLevel)
		scene.next_level();
	ACTION_IMPL_END(ActionID::NextLevel)
//--------------------------------------------------------------------------------------------------
	ACTION_IMPL_BEGIN(ActionID::PreviousLevel)
		scene.previous_level();
	ACTION_IMPL_END(ActionID::PreviousLevel)
//--------------------------------------------------------------------------------------------------
}
