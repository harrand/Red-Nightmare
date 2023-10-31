#include "gamelib/entity/api.hpp"
#include "gamelib/entity/scene.hpp"
#include "tz/dbgui/dbgui.hpp"

namespace game::entity
{
	std::size_t entity::uid_global_counter = 0;

	game::logic::stats entity::get_stats() const
	{
		game::logic::stats ret = this->base_stats;
		// level increases stats too.
		// 5% max health per level up.
		ret.maximum_health *= 1.0f + ((this->level - 1.0f) * 0.05f);

		for(const auto&[_, buff] : this->buffs)
		{
			ret = ret + buff;
		}
		return ret;
	}

	relationship entity::get_relationship(const entity& rhs) const
	{
		return this->get_relationship(rhs.allegience);
	}

	relationship entity::get_relationship(faction allegience) const
	{
		if(this->allegience == faction::pure_friend || allegience == faction::pure_friend)
		{
			return relationship::friendly;
		}
		if(this->allegience == faction::pure_neutral || allegience == faction::pure_neutral)
		{
			return relationship::neutral;
		}
		if(this->allegience == faction::pure_enemy || allegience == faction::pure_enemy)
		{
			return relationship::hostile;
		}
		tz::assert(this->allegience == faction::player_ally || this->allegience == faction::player_enemy);
		tz::assert(allegience == faction::player_ally || allegience == faction::player_enemy);
		if(this->allegience == allegience)
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

	void entity::level_up()
	{
		if(this->level < std::numeric_limits<std::uint8_t>::max())
		{
			this->level++;
			this->current_health = this->get_stats().maximum_health;
		}
	}

	void entity::set_level(std::uint8_t new_level)
	{
		if(new_level <= this->level)
		{
			this->level = new_level;
			this->current_health = std::min(this->current_health, this->get_stats().maximum_health);
		}
		else
		{
			while(this->level < new_level)
			{
				this->level_up();
			}
		}
	}

	/*static*/ std::string entity::get_type_name(std::size_t type)
	{
		auto& state = tz::lua::get_state();	
		std::string cmd = "_tmp_type_id = " + std::to_string(type);
		state.execute(cmd.c_str());
		state.execute("rn.get_entity_type_name()");
		std::optional<std::string> ret = state.get_string("_tmp_type_name");
		tz::assert(ret.has_value());
		return ret.value();
	}

	/*static*/ std::size_t entity::get_type_count()
	{
		auto& state = tz::lua::get_state();	
		state.execute("rn.get_entity_type_count()");
		std::optional<std::uint64_t> ret = state.get_uint("_tmp_type_count");
		tz::assert(ret.has_value());
		return ret.value();
	}


	void entity::dbgui()
	{
		std::string type_name = entity::get_type_name(this->type);
		ImGui::Text("Type: %zu (%s)", this->type, type_name.c_str());
		ImGui::Text("Name: %s", this->name.c_str());
		ImGui::Spacing();
		tz::trs trs = this->elem.renderer->get_renderer().animated_object_get_local_transform(this->elem.entry.obj);
		ImGui::Text("Position");
		if(ImGui::InputFloat2("##pos", trs.translate.data().data()))
		{
			this->elem.renderer->get_renderer().animated_object_set_local_transform(this->elem.entry.obj, trs);
		}
		const float max_hp = this->get_stats().maximum_health;
		ImGui::SliderInt("Health", reinterpret_cast<int*>(&this->current_health), 0u, max_hp);
		int tmp_level = this->level;
		if(ImGui::SliderInt("Level", &tmp_level, 1, std::numeric_limits<std::uint8_t>::max()))
		{
			this->set_level(tmp_level);
		}
		ImGui::SameLine();
		if(ImGui::Button("+"))
		{
			this->level_up();
		}

		ImGui::Separator();
		ImGui::Text("Faction: "); ImGui::SameLine();	
		ImVec4 faction_col;
		const char* faction_str = "<unknown faction>";
		switch(this->get_relationship(faction::player_ally))
		{
			case relationship::friendly:
				faction_col = ImVec4{0.2f, 0.9f, 0.2f, 1.0f};
			break;
			case relationship::neutral:
				faction_col = ImVec4{0.8f, 0.8f, 0.0f, 1.0f};
			break;
			case relationship::hostile:
				faction_col = ImVec4{1.0f, 0.1f, 0.1f, 1.0f};
			break;
		}
		ImGui::TextColored(faction_col, game::entity::faction_names[static_cast<int>(this->allegience)]);
	}

	// Lua API

	entity& rn_impl_entity::get()
	{
		return this->scene->get(this->entity_hanval);
	}
	
	int rn_impl_entity::is_valid(tz::lua::state& state)
	{
		state.stack_push_bool(this->get().type != std::numeric_limits<std::size_t>::max());
		return 1;
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

	int rn_impl_entity::get_level(tz::lua::state& state)
	{
		state.stack_push_uint(this->get().level);
		return 1;
	}

	int rn_impl_entity::set_level(tz::lua::state& state)
	{
		auto [_, new_level] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		constexpr auto lvl_max = std::numeric_limits<std::uint8_t>::max();
		if(new_level > lvl_max)
		{
			new_level = lvl_max;
			tz::report("Warning: Setting level to %u is invalid. Max level is %u", new_level, lvl_max);
		}
		this->get().set_level(new_level);
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