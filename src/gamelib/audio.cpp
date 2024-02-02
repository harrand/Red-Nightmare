#include "gamelib/audio.hpp"
#include "tz/lua/api.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include "tz/lua/state.hpp"

#include <filesystem>
#include <array>

namespace game
{
	SoLoud::Soloud soloud;
	std::unordered_map<std::string, SoLoud::Wav> wavs;
	std::array<SoLoud::handle, 8> music_tracks;

	void audio_initialise()
	{
		soloud.init();
		for(auto& track : music_tracks)
		{
			track = -1;
		}
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

	LUA_BEGIN(lua_play_music)
		auto [path, track] = tz::lua::parse_args<std::string, unsigned int>(state);
		float volume = 1.0f;
		tz::lua::lua_generic maybe_volume = state.stack_get_generic(3);
		if(std::holds_alternative<double>(maybe_volume))
		{
			volume = std::get<double>(maybe_volume);
		}
		std::string cwd = std::filesystem::current_path().string();
		path = cwd + std::string("/res/sounds/") + path;
		play_music(path.c_str(), track, volume);
		return 0;
	LUA_END

	LUA_BEGIN(lua_stop_music)
		auto [track] = tz::lua::parse_args<unsigned int>(state);
		stop_music(track);
		return 0;
	LUA_END

	void audio_lua_initialise(tz::lua::state& state)
	{
		state.assign_func("rn.play_sound", LUA_FN_NAME(lua_play_sound));
		state.assign_func("rn.play_music", LUA_FN_NAME(lua_play_music));
		state.assign_func("rn.stop_music", LUA_FN_NAME(lua_stop_music));
	}

	void play_sound(const char* path, float volume)
	{
		if(wavs.find(path) == wavs.end())
		{
			wavs[path].load(path);
		}
		soloud.play(wavs[path], volume);
	}

	void play_music(const char* path, std::size_t track_id, float volume)
	{
		if(wavs.find(path) == wavs.end())
		{
			wavs[path].load(path);
			wavs[path].setLooping(true);
		}
		tz::assert(music_tracks.size() > track_id);
		music_tracks[track_id] = soloud.play(wavs[path]);
	}

	void stop_music(std::size_t track_id)
	{
		tz::assert(music_tracks.size() > track_id);
		soloud.stop(music_tracks[track_id]);
	}
}