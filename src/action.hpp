#ifndef REDNIGHTMARE_ACTION_HPP
#define REDNIGHTMARE_ACTION_HPP
#include "entity.hpp"
#include "hdk/data/vector.hpp"
#include "hdk/memory/clone.hpp"
#include <vector>
#include <memory>

namespace game
{
	// Pre-declares.
	enum class ActorType;
	enum class BuffID;
	class SceneData;

	/// Actions cause an actor to do something until the action is considered complete. Some actions occur instantly, but many do not.
	enum class ActionID
	{
		/**
		 * Move to the mouse cursor's location.
		 * - Completes instantly. Spawns a GotoTarget action in its stead.
		 */
		GotoMouse,
		/**
		 * Move to a player's location.
		 * - If there are no players, this does nothing.
		 * - Completes instantly. Spawns a GotoTarget action in its stead.
		 */
		GotoPlayer,
		/**
		 * Move to an actor's location.
		 * - Completes instantly. Spawns a GotoTarget action in its stead.
		 */
		GotoActor,
		/**
		 * Move to the target location.
		 * - Completes when the actor arrives at the location.
		 */
		GotoTarget,
		/**
		 * Launch towards the mouse cursor with a speed boost.
		 * - Moves proportionally to base speed.
		 * - Never completes, unless the actor goes out-of-bounds.
		 */
		LaunchToMouse,
		/**
		 * Launch towards a player with a speed boost.
		 * - Moves proportionally to base speed.
		 * - Never completes, unless the actor goes out-of-bounds.
		 */
		LaunchToPlayer,
		/**
		 * Launch towards the target location with a speed boost.
		 * - Moves proportionally to base speed.
		 * - Never completes, unless the actor goes out-of-bounds.
		 */
		Launch,
		/// Teleport to the player. If there are multiple players, the player with the lowest actor-id is chosen.
		TeleportToPlayer,
		/// Teleport to a random location in the current level. Respects scene boundaries.
		RandomTeleport,
		/// Teleport to the target location.
		Teleport,
		/// Flip the actor's sprite horizontally.
		HorizontalFlip,
		/// Flip the actor's sprite vertically.
		VerticalFlip,
		/// Spawn an actor as soon as possible once.
		SpawnActor,
		/// Respawn the current actor.
		Respawn,
		/// Respawn the current actor, but as a different actor type.
		RespawnAs,
		/// Instantly kill the current actor.
		Die,
		/// Apply a buff to the current actor.
		ApplyBuff,
		/// Apply a buff to another actor.
		ApplyBuffToActor,
		/// Apply a buff to the target, if there is one.
		ApplyBuffToTarget,
		/// Apply a buff to all living players.
		ApplyBuffToPlayers,
	};

	template<ActionID ID>
	struct ActionParams{};

	class IAction : public hdk::unique_cloneable<IAction>
	{
	public:
		virtual constexpr ActionID get_id() const = 0;
		bool get_is_complete() const{return this->is_complete;}
		void set_is_complete(bool is_complete){this->is_complete = is_complete;}
	private:
		bool is_complete = false;
	};

	template<ActionID ID>
	class Action : public IAction
	{
	public:
		Action(ActionParams<ID> params = {}): params(params){}
		[[nodiscard]] virtual std::unique_ptr<IAction> unique_clone() const
		{
			return static_cast<std::unique_ptr<IAction>>(std::make_unique<Action<ID>>(*this));
		}
		virtual constexpr ActionID get_id() const override{return ID;}
		const ActionParams<ID>& data() const{return this->params;}
		ActionParams<ID>& data(){return this->params;}
	private:
		ActionParams<ID> params;
	};

	// Action implementations.
	
	template<>
	struct ActionParams<ActionID::GotoActor>
	{
		std::size_t actor_id;
	};
	
	template<>
	struct ActionParams<ActionID::GotoTarget>
	{
		hdk::vec2 target_position;
		float timeout = std::numeric_limits<float>::max();
	};

	template<>
	struct ActionParams<ActionID::LaunchToMouse>
	{
		float speed_multiplier = 1.0f;
	};

	template<>
	struct ActionParams<ActionID::LaunchToPlayer>
	{
		float speed_multiplier = 1.0f;
	};

	template<>
	struct ActionParams<ActionID::Launch>
	{
		hdk::vec2 direction;
		float speed_multiplier = 1.0f;
	};

	template<>
	struct ActionParams<ActionID::Teleport>
	{
		hdk::vec2 position;
	};

	template<>
	struct ActionParams<ActionID::SpawnActor>
	{
		ActorType actor;
		bool inherit_faction = false;
	};

	template<>
	struct ActionParams<ActionID::RespawnAs>
	{
		ActorType actor;
	};

	template<>
	struct ActionParams<ActionID::ApplyBuff>
	{
		BuffID buff;
	};

	template<>
	struct ActionParams<ActionID::ApplyBuffToActor>
	{
		std::size_t actor_id;
		BuffID buff;
	};

	template<>
	struct ActionParams<ActionID::ApplyBuffToTarget>
	{
		BuffID buff;
	};

	template<>
	struct ActionParams<ActionID::ApplyBuffToPlayers>
	{
		BuffID buff;
	};

	template<ActionID ID>
	void action_invoke(SceneData& scene, Action<ID>& action);

	/// Represents something that can carry out actions.
	class ActionEntity : public Entity<ActionID, IAction, Action, ActionParams>
	{
	public:
		ActionEntity() = default;
		using Entity<ActionID, IAction, Action, ActionParams>::Entity;
		virtual void update() override;
	};
}
#include "action.inl"

#endif // REDNIGHTMARE_ACTION_HPP
