#ifndef REDNIGHTMARE_GAMELIB_AUDIO_HPP
#define REDNIGHTMARE_GAMELIB_AUDIO_HPP
#include "tz/lua/state.hpp"
#include <cstddef>

namespace game
{
	void audio_initialise();
	void audio_terminate();
	void audio_lua_initialise(tz::lua::state& state);
	void audio_set_global_volume(float volume);
	float audio_get_global_volume();
	void play_sound(const char* path, float volume = 1.0f);
	void play_music(const char* path, std::size_t track_id, float volume = 1.0f);
	void stop_music(std::size_t track_id);
}

#endif // REDNIGHTMARE_GAMELIB_AUDIO_HPP