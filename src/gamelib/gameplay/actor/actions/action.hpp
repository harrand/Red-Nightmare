#ifndef RNLIB_GAMEPLAY_ACTOR_ACTIONS_ACTION_HPP
#define RNLIB_GAMEPLAY_ACTOR_ACTIONS_ACTION_HPP
#include "gamelib/core/entity.hpp"
#include "tz/core/memory/clone.hpp"
#include "tz/core/data/vector.hpp"

namespace rnlib
{
	enum class action_id
	{
		teleport,
		random_teleport,
		_count
	};

	template<action_id ID>
	struct action_params{};

	template<>
	struct action_params<action_id::teleport>
	{
		tz::vec2 location;
	};

	class iaction : public tz::unique_cloneable<iaction>
	{
	public:
		virtual constexpr action_id get_id() const = 0;
		virtual ~iaction() = default;
		void set_is_complete(bool complete){this->is_complete = complete;}
		bool get_is_complete() const{return this->is_complete;}
	private:
		bool is_complete = false;
	};

	template<action_id ID>
	class action : public iaction
	{
	public:
		action(action_params<ID> params = {}): params(std::move(params)){}
		[[nodiscard]] virtual std::unique_ptr<iaction> unique_clone() const
		{
			return static_cast<std::unique_ptr<iaction>>(std::make_unique<action<ID>>(*this));
		}
		virtual constexpr action_id get_id() const override{return ID;}
		const action_params<ID>& data() const{return this->params;}
		action_params<ID>& data(){return this->params;}
	private:
		action_params<ID> params;
	};

	class action_entity : public rnlib::entity<action_id, iaction, action, action_params>
	{
	public:
		using rnlib::entity<action_id, iaction, action, action_params>::entity;
		void update();
		void dbgui();
	};

	class actor_system;
	class actor;

	template<action_id ID>
	void action_invoke(actor_system& system, actor& caster, action<ID>& action);
}

#endif //RNLIB_GAMEPLAY_ACTOR_ACTIONS_ACTION_HPP
