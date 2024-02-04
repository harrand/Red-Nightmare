#ifndef REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP
#define REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP
#include "tz/lua/state.hpp"
#include "tz/wsi/keyboard.hpp"

namespace game::input
{
	void keyboard_initialise();
	void keyboard_advance();
	bool is_key_down(tz::wsi::key k);

	void keyboard_lua_initialise(tz::lua::state& state);
}

#endif // REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP