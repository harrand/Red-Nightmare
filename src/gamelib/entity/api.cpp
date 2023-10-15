#include "gamelib/entity/api.hpp"
#include "gamelib/entity/scene.hpp"

namespace game::entity
{
	std::size_t entity::uid_global_counter = 0;

	game::logic::stats entity::get_stats() const
	{
		game::logic::stats ret = this->base_stats;
		for(const auto&[_, buff] : this->buffs)
		{
			ret = ret + buff;
		}
		return ret;
	}

	relationship entity::get_relationship(const entity& rhs) const
	{
		if(this->allegience == faction::pure_friend || rhs.allegience == faction::pure_friend)
		{
			return relationship::friendly;
		}
		if(this->allegience == faction::pure_neutral || rhs.allegience == faction::pure_neutral)
		{
			return relationship::neutral;
		}
		if(this->allegience == faction::pure_enemy || rhs.allegience == faction::pure_enemy)
		{
			return relationship::hostile;
		}
		tz::assert(this->allegience == faction::player_ally || this->allegience == faction::player_enemy);
		tz::assert(rhs.allegience == faction::player_ally || rhs.allegience == faction::player_enemy);
		if(this->allegience == rhs.allegience)
		{
			return relationship::friendly;
		}	
		else
		{
			return relationship::hostile;
		}
	}

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

	int rn_impl_entity::get_faction(tz::lua::state& state)
	{
		state.stack_push_int(static_cast<int>(this->get().allegience));
		return 1;
	}

	int rn_impl_entity::set_faction(tz::lua::state& state)
	{
		auto [_, facval] = tz::lua::parse_args<tz::lua::nil, int>(state);
		tz::assert(facval >= 0 && facval <= 4, "Invalid faction value %d. Must be between 0-4", facval);
		this->get().allegience = static_cast<faction>(facval);
		return 0;
	}

	int rn_impl_entity::get_relationship(tz::lua::state& state)
	{
		auto& rhs_ent = state.stack_get_userdata<rn_impl_entity>(2);
		relationship ret = this->get().get_relationship(rhs_ent.get());
		state.stack_push_int(static_cast<int>(ret));
		return 1;
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
		auto s = this->get().get_stats();
		LUA_CLASS_PUSH(state, rn_impl_stats, {.s = s});
		return 1;
	}

	int rn_impl_entity::get_health(tz::lua::state& state)
	{
		state.stack_push_uint(this->get().current_health);
		return 1;
	}

	int rn_impl_entity::set_health(tz::lua::state& state)
	{
		auto [_, hp] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->get().current_health = std::clamp(hp, 0u, static_cast<unsigned int>(this->get().get_stats().maximum_health));
		return 0;
	}

	int rn_impl_entity::is_dead(tz::lua::state& state)
	{
		state.stack_push_bool(this->get().current_health == 0);
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