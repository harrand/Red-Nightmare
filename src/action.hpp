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
		ChaseTarget
	};

	template<ActionID ID>
	struct ActionParams{};

	class IAction
	{
	public:
		virtual constexpr ActionID get_id() const = 0;
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

	/// Represents something that can carry out actions.
	class ActionEntity
	{
	public:
		ActionEntity(const ActionEntity& copy) = delete;
		ActionEntity(ActionEntity&& move) = default;
		ActionEntity() = default;
		ActionEntity& operator=(const ActionEntity& rhs) = delete;
		ActionEntity& operator=(ActionEntity&& rhs) = default;

		using Handle = tz::Handle<ActionEntity>;
		template<ActionID ID>
		Handle add(ActionParams<ID> params);
		template<ActionID ID>
		bool has() const;
		template<ActionID ID>
		const Action<ID>* get() const;
		template<ActionID ID>
		Action<ID>* get();
	private:
		std::vector<std::unique_ptr<IAction>> actions;
	};
}
#include "action.inl"

#endif // REDNIGHTMARE_ACTION_HPP
