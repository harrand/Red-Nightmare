#ifndef RN_GAMELIB_RNLIB_HPP
#define RN_GAMELIB_RNLIB_HPP
#include <cstdint>

namespace game
{
	void initialise();	
	void terminate();
	void update(std::uint64_t delta_micros);
	void fixed_update(std::uint64_t delta_micros, std::uint64_t unprocessed);
	void dbgui();
	void dbgui_game_bar();
}

#endif // RN_GAMELIB_RNLIB_HPP