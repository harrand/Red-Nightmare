#ifndef REDNIGHTMARE_GAMELIB_AUDIO_HPP
#define REDNIGHTMARE_GAMELIB_AUDIO_HPP

namespace game
{
	void audio_initialise();
	void audio_terminate();
	void audio_lua_initialise();
	void play_sound(const char* path, float volume = 1.0f);
}

#endif // REDNIGHTMARE_GAMELIB_AUDIO_HPP