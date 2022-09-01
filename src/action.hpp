#ifndef REDNIGHTMARE_ACTION_HPP
#define REDNIGHTMARE_ACTION_HPP
#include "tz/core/vector.hpp"
#include "tz/core/handle.hpp"
#include <vector>
#include <memory>

namespace game
{
	enum class ActionID
	{
		ChaseMouse,
		ChaseTarget,
		LaunchToMouse,
		Launch,
		TeleportToPlayer,
		Teleport,
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
	struct ActionParams<ActionID::ChaseTarget>
	{
		tz::Vec2 target_position;
	};

	template<>
	struct ActionParams<ActionID::Launch>
	{
		tz::Vec2 direction;
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
