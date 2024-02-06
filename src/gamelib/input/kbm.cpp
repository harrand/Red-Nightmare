#include "gamelib/input/kbm.hpp"
#include "tz/lua/api.hpp"
#include "tz/core/profile.hpp"
#include "tz/tz.hpp"
#include "tz/wsi/keyboard.hpp"
#include "tz/wsi/mouse.hpp"
#include "tz/dbgui/dbgui.hpp"
#include <algorithm>

namespace game::input
{
	std::unordered_map<std::string, tz::wsi::key> internal_key_name_mappings = {};
	std::array<bool, static_cast<std::size_t>(tz::wsi::key::_count)> internal_key_states;

	std::unordered_map<std::string, tz::wsi::mouse_button> internal_mouse_button_name_mappings = {};
	std::array<bool, static_cast<std::size_t>(tz::wsi::mouse_button::_count)> internal_mouse_button_states;

	void input_initialise()
	{
		for(std::size_t i = 0; i < static_cast<std::size_t>(tz::wsi::key::_count); i++)
		{
			auto k = static_cast<tz::wsi::key>(i);
			std::string keyname = tz::wsi::get_key_name(k);
			std::transform(keyname.begin(), keyname.end(), keyname.begin(), [](char c){return std::tolower(c);});
			// for some reason, these wsi strings contain a bunch of duplicate null terminators.
			// this is big problem because "morb\0\0\0" != "morb" even though they look exactly the same.
			// strip them all out:
			keyname.erase(std::remove(keyname.begin(), keyname.end(), '\0'), keyname.end());
			internal_key_name_mappings[keyname] = k;
			internal_key_states[i] = false;
		}

		for(std::size_t i = 0; i < static_cast<std::size_t>(tz::wsi::mouse_button::_count); i++)
		{
			auto m = static_cast<tz::wsi::mouse_button>(i);
			std::string mname = "Unknown";
			switch(m)
			{
				case tz::wsi::mouse_button::left:
					mname = "left";
				break;
				case tz::wsi::mouse_button::middle:
					mname = "middle";
				break;
				case tz::wsi::mouse_button::right:
					mname = "right";
				break;
				default:
					tz::error();
				break;
			}
			internal_mouse_button_name_mappings[mname] = m;
			internal_mouse_button_states[i] = false;
		}

	}

	void input_advance()
	{
		const auto& kbstate = tz::window().get_keyboard_state();
		for(std::size_t i = 0; i < static_cast<std::size_t>(tz::wsi::key::_count); i++)
		{
			auto k = static_cast<tz::wsi::key>(i);
			internal_key_states[i] = tz::wsi::is_key_down(kbstate, k);
		}
		
		const auto& mstate = tz::window().get_mouse_state();
		for(std::size_t i = 0; i < static_cast<std::size_t>(tz::wsi::mouse_button::_count); i++)
		{
			auto m = static_cast<tz::wsi::mouse_button>(i);
			internal_mouse_button_states[i] = tz::wsi::is_mouse_button_down(mstate, m);
		}
	}

	bool is_key_down(tz::wsi::key k)
	{
		return !tz::dbgui::claims_keyboard() && internal_key_states[static_cast<int>(k)];
	}

	bool is_mouse_button_down(tz::wsi::mouse_button b)
	{
		return !tz::dbgui::claims_mouse() && internal_mouse_button_states[static_cast<int>(b)];
	}

	// Lua API

	struct impl_rn_kbm_input
	{
		int is_key_down(tz::lua::state& state)
		{
			auto [_, keyname] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
			// lowercase the arg.
			std::transform(keyname.begin(), keyname.end(), keyname.begin(), [](char c){return std::tolower(c);});
			auto iter = internal_key_name_mappings.find(keyname);
			tz::assert(iter != internal_key_name_mappings.end(), "Unknown key name \"%s\"", keyname.c_str());
			state.stack_push_bool(game::input::is_key_down(iter->second));
			return 1;
		}

		int is_mouse_down(tz::lua::state& state)
		{
			auto [_, mname] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
			// lowercase the arg.
			std::transform(mname.begin(), mname.end(), mname.begin(), [](char c){return std::tolower(c);});
			auto iter = internal_mouse_button_name_mappings.find(mname);
			tz::assert(iter != internal_mouse_button_name_mappings.end(), "Unknown mouse button name \"%s\"", mname.c_str());
			state.stack_push_bool(game::input::is_mouse_button_down(iter->second));
			return 1;
		}
	};

	LUA_CLASS_BEGIN(impl_rn_kbm_input)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_kbm_input, is_key_down)
			LUA_METHOD(impl_rn_kbm_input, is_mouse_down)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(rn_input)
		LUA_CLASS_PUSH(state, impl_rn_kbm_input, {});
		return 1;
	LUA_END

	void input_lua_initialise(tz::lua::state& state)
	{
		TZ_PROFZONE("keyboard/mouse - lua initialise", 0xFFFF4488);
		state.new_type("impl_rn_kbm_input", LUA_CLASS_NAME(impl_rn_kbm_input)::registers);
		state.execute("rn = rn or {}");
		state.assign_func("rn.input", LUA_FN_NAME(rn_input));
	}
}