#ifndef REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP
#define REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP
#include "tz/lua/state.hpp"
#include "tz/wsi/keyboard.hpp"

namespace game::input
{
	void input_initialise();
	void input_advance();
	bool is_key_down(tz::wsi::key k);
	bool is_mouse_button_down(tz::wsi::key k);

	void input_lua_initialise(tz::lua::state& state);
}

#endif // REDNIGHTMARE_GAMELIB_INPUT_KEYBOARD_HPP