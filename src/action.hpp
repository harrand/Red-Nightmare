#ifndef REDNIGHTMARE_ACTION_HPP
#define REDNIGHTMARE_ACTION_HPP
#include "tz/core/vector.hpp"
#include "tz/core/handle.hpp"
#include <vector>
#include <memory>

namespace game
{
	/// Actions cause an actor to do something until the action is considered complete. Some actions occur instantly, but many do not.
	enum class ActionID
	{
		/**
		 * Move to the mouse cursor's location.
		 * - Always moves at base movement speed.
		 * - Completes when the actor arrives at the location.
		 */
		GotoMouse,
		/**
		 * Move to the target location.
		 * - Always moves at base movement speed.
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
		 * Launch towards the target location with a speed boost.
		 * - Moves proportionally to base speed.
		 * - Never completes, unless the actor goes out-of-bounds.
		 */
		Launch,
		/// Teleport to the player. If there are multiple players, the player with the lowest actor-id is chosen.
		TeleportToPlayer,
		/// Teleport to the target location.
		Teleport,
		/// Flip the actor's sprite horizontally.
		HorizontalFlip,
		/// Flip the actor's sprite vertically.
		VerticalFlip,
	};

	template<ActionID ID>
	struct ActionParams{};

	class IAction
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
		Action(ActionParams<ID> params): params(params){}
		virtual constexpr ActionID get_id() const override{return ID;}
		const ActionParams<ID>& data() const{return this->params;}
		ActionParams<ID>& data(){return this->params;}
	private:
		ActionParams<ID> params;
	};

	// Action implementations.
	
	template<>
	struct ActionParams<ActionID::GotoTarget>
	{
		tz::Vec2 target_position;
	};

	template<>
	struct ActionParams<ActionID::LaunchToMouse>
	{
		float speed_multiplier = 1.0f;
	};

	template<>
	struct ActionParams<ActionID::Launch>
	{
		tz::Vec2 direction;
		float speed_multiplier = 1.0f;
	};

	template<>
	struct ActionParams<ActionID::Teleport>
	{
		tz::Vec2 position;
	};

	/// Represents something that can carry out actions.
	class ActionEntity
	{
	public:
		ActionEntity(const ActionEntity& copy) = delete;
		ActionEntity(ActionEntity&& move) = default;
		ActionEntity() = default;
		ActionEntity& operator=(const ActionEntity& rhs) = delete;
		ActionEntity& operator=(ActionEntity&& rhs) = default;

		template<ActionID ID>
		bool add(ActionParams<ID> params = {});

		template<ActionID ID>
		bool set(ActionParams<ID> params = {});

		template<ActionID ID>
		bool has() const;

		template<ActionID ID>
		const Action<ID>* get() const;
		template<ActionID ID>
		Action<ID>* get();

		std::size_t size() const;
		void update();
	private:
		std::vector<std::unique_ptr<IAction>> actions;
	};
}
#include "action.inl"

#endif // REDNIGHTMARE_ACTION_HPP
