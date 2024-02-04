#include "gamelib/input/keyboard.hpp"
#include "tz/lua/api.hpp"
#include "tz/core/profile.hpp"
#include "tz/tz.hpp"
#include "tz/wsi/keyboard.hpp"
#include <algorithm>

namespace game::input
{
	std::unordered_map<std::string, tz::wsi::key> internal_key_name_mappings = {};
	std::array<bool, static_cast<std::size_t>(tz::wsi::key::_count)> internal_key_states;

	void keyboard_initialise()
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

	}

	void keyboard_advance()
	{
		const auto& kbstate = tz::window().get_keyboard_state();
		for(std::size_t i = 0; i < static_cast<std::size_t>(tz::wsi::key::_count); i++)
		{
			auto k = static_cast<tz::wsi::key>(i);
			internal_key_states[i] = tz::wsi::is_key_down(kbstate, k);
		}
	}

	bool is_key_down(tz::wsi::key k)
	{
		return internal_key_states[static_cast<int>(k)];
	}

	// Lua API

	struct impl_rn_keyboard_input
	{
		int is_key_down(tz::lua::state& state)
		{
			auto [_, keyname] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
			// lowercase the arg.
			std::transform(keyname.begin(), keyname.end(), keyname.begin(), [](char c){return std::tolower(c);});
			auto iter = internal_key_name_mappings.find(keyname);
			tz::assert(iter != internal_key_name_mappings.end(), "Unknown key name \"%s\"", keyname.c_str());
			state.stack_push_bool(internal_key_states[static_cast<std::size_t>(iter->second)]);
			return 1;
		}
	};

	LUA_CLASS_BEGIN(impl_rn_keyboard_input)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(impl_rn_keyboard_input, is_key_down)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(rn_keyboard)
		LUA_CLASS_PUSH(state, impl_rn_keyboard_input, {});
		return 1;
	LUA_END

	void keyboard_lua_initialise(tz::lua::state& state)
	{
		TZ_PROFZONE("keyboard input - lua initialise", 0xFFFF4488);
		state.new_type("impl_rn_keyboard_input", LUA_CLASS_NAME(impl_rn_keyboard_input)::registers);
		state.execute("rn = rn or {}; rn.input = rn.input or {}");
		state.assign_func("rn.input.keyboard", LUA_FN_NAME(rn_keyboard));
	}
}