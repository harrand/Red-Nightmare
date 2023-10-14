#include "gamelib/entity/api.hpp"
#include "gamelib/entity/scene.hpp"

namespace game::entity
{
	std::size_t entity::uid_global_counter = 0;

	void entity::update(float delta_seconds)
	{
		for(auto iter = this->buffs.begin(); iter != this->buffs.end();)
		{
			auto& buff = iter->second;
			if(!buff.time_remaining_seconds.has_value())
			{
				iter++;
				continue;
			}
			float& t = buff.time_remaining_seconds.value();
			t -= delta_seconds;
			if(t < 0.0f)
			{
				// time to remove the buff.
				tz::report("Buff %s has timed out.", buff.name.c_str());
				iter = this->buffs.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}

	entity& rn_impl_entity::get()
	{
		return this->scene->get(this->entity_hanval);
	}

	int rn_impl_entity::get_type(tz::lua::state& state)
	{
		state.stack_push_uint(this->get().type);
		return 1;
	}

	int rn_impl_entity::uid(tz::lua::state& state)
	{
		state.stack_push_uint(this->get().uid);
		return 1;
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

	int rn_impl_entity::get_base_stats(tz::lua::state& state)
	{
		using namespace game::logic;
		LUA_CLASS_PUSH(state, rn_impl_stats, {.s = this->get().base_stats});
		return 1;
	}

	int rn_impl_entity::set_base_stats(tz::lua::state& state)
	{
		using namespace game::logic;
		auto& stats = state.stack_get_userdata<rn_impl_stats>(2);
		this->get().base_stats = stats.s;
		return 0;
	}

	int rn_impl_entity::get_stats(tz::lua::state& state)
	{
		using namespace game::logic;
		auto s = this->get().base_stats;
		for(const auto& [_, buff] : this->get().buffs)
		{
			s = s + buff;
		}
		LUA_CLASS_PUSH(state, rn_impl_stats, {.s = s});
		return 1;
	}

	int rn_impl_entity::apply_buff(tz::lua::state& state)
	{
		using namespace game::logic;
		auto& buff = state.stack_get_userdata<rn_impl_buff>(2);
		this->get().buffs[buff.b.name] = buff.b;
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