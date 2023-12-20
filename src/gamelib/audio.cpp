#include "gamelib/audio.hpp"
#include "tz/lua/api.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include "tz/lua/state.hpp"

#include <filesystem>

namespace game
{
	SoLoud::Soloud soloud;
	std::unordered_map<std::string, SoLoud::Wav> wavs;

	void audio_initialise()
	{
		soloud.init();
	}
	
	void audio_terminate()
	{
		soloud.deinit();
	}

	LUA_BEGIN(lua_play_sound)
		auto [path] = tz::lua::parse_args<std::string>(state);
		float volume = 1.0f;
		tz::lua::lua_generic maybe_volume = state.stack_get_generic(2);
		if(std::holds_alternative<double>(maybe_volume))
		{
			volume = std::get<double>(maybe_volume);
		}
		std::string cwd = std::filesystem::current_path().string();
		path = cwd + std::string("/res/sounds/") + path;
		play_sound(path.c_str(), volume);
		return 0;
	LUA_END

	void audio_lua_initialise()
	{
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			state.assign_func("rn.play_sound", LUA_FN_NAME(lua_play_sound));
		});
	}

	void play_sound(const char* path, float volume)
	{
		if(wavs.find(path) == wavs.end())
		{
			wavs[path].load(path);
		}
		soloud.play(wavs[path], volume);
	}
}