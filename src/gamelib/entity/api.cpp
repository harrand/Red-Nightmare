#include "gamelib/entity/api.hpp"
#include "gamelib/entity/scene.hpp"

namespace game::entity
{
	entity& rn_impl_entity::get()
	{
		return this->scene->get(this->entity_hanval);
	}

	int rn_impl_entity::get_name(tz::lua::state& state)
	{
		state.stack_push_string(this->get().name);
		return 1;
	}

	int rn_impl_entity::set_name(tz::lua::state& state)
	{
		auto [_, name] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
		this->get().name = name;
		return 0;
	}

	int rn_impl_entity::get_model(tz::lua::state& state)
	{
		state.stack_push_int(static_cast<int>(this->get().elem.entry.m));
		return 1;
	}

	int rn_impl_entity::set_model(tz::lua::state& state)
	{
		auto [_, modelval] = tz::lua::parse_args<tz::lua::nil, int>(state);
		this->get().elem.entry.m = static_cast<render::scene_renderer::model>(modelval);
		return 0;
	}

	int rn_impl_entity::get_element(tz::lua::state& state)
	{
		using namespace game::render;
		LUA_CLASS_PUSH(state, impl_rn_scene_element, {.elem = this->get().elem});
		return 1;
	}
}