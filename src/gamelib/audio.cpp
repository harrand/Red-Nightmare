#include "gamelib/audio.hpp"
#include "gamelib/messaging/scene.hpp"
#include "tz/lua/api.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include "tz/lua/state.hpp"

#include <filesystem>
#include <array>

namespace game
{
	constexpr SoLoud::handle audio_nullhand = std::numeric_limits<SoLoud::handle>::max();
	SoLoud::Soloud soloud;
	std::unordered_map<std::string, SoLoud::Wav> wavs;
	std::array<SoLoud::handle, 8> music_tracks;

	void audio_initialise()
	{
		soloud.init();
		for(auto& track : music_tracks)
		{
			track = audio_nullhand;
		}
	}
	
	void audio_terminate()
	{
		soloud.deinit();
	}

	bool music_is_playing(std::size_t track_id);

	LUA_BEGIN(lua_play_sound)
		auto [path] = tz::lua::parse_args<std::string>(state);
		float volume = 1.0f;
		if(state.stack_size() >= 2)
		{
			tz::lua::lua_generic maybe_volume = state.stack_get_generic(2);
			if(std::holds_alternative<double>(maybe_volume))
			{
				volume = std::get<double>(maybe_volume);
			}
		}
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::audio_play_sound,
			.uuid = std::numeric_limits<entity_uuid>::max(),
			.value = std::pair<std::string, float>{path, volume}
		});
		return 0;
	LUA_END

	LUA_BEGIN(lua_play_music)
		auto [path, track] = tz::lua::parse_args<std::string, unsigned int>(state);
		float volume = 1.0f;
		if(state.stack_size() >= 3)
		{
			tz::lua::lua_generic maybe_volume = state.stack_get_generic(3);
			if(std::holds_alternative<double>(maybe_volume))
			{
				volume = std::get<double>(maybe_volume);
			}
		}
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::audio_play_music,
			.uuid = std::numeric_limits<entity_uuid>::max(),
			.value = std::tuple<std::string, unsigned int, float>{path, track, volume}
		});
		return 0;
	LUA_END

	LUA_BEGIN(lua_music_is_playing)
		auto [track] = tz::lua::parse_args<unsigned int>(state);
		state.stack_push_bool(music_is_playing(track));
		return 1;
	LUA_END

	LUA_BEGIN(lua_stop_music)
		auto [track] = tz::lua::parse_args<unsigned int>(state);
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::audio_stop_music,
			.uuid = std::numeric_limits<entity_uuid>::max(),
			.value = track
		});
		return 0;
	LUA_END

	void audio_lua_initialise(tz::lua::state& state)
	{
		state.assign_func("rn.play_sound", LUA_FN_NAME(lua_play_sound));
		state.assign_func("rn.play_music", LUA_FN_NAME(lua_play_music));
		state.assign_func("rn.music_is_playing", LUA_FN_NAME(lua_music_is_playing));
		state.assign_func("rn.stop_music", LUA_FN_NAME(lua_stop_music));
	}

	float global_volume = 0.1f;

	void audio_set_global_volume(float volume)
	{
		global_volume = volume;
	}

	float audio_get_global_volume()
	{
		return global_volume;
	}

	void play_sound(const char* path, float volume)
	{
		if(wavs.find(path) == wavs.end())
		{
			wavs[path].load(path);
		}
		soloud.play(wavs[path], volume * global_volume);
	}

	void play_music(const char* path, std::size_t track_id, float volume)
	{
		if(wavs.find(path) == wavs.end())
		{
			wavs[path].load(path);
			wavs[path].setLooping(true);
		}
		tz::assert(music_tracks.size() > track_id);
		music_tracks[track_id] = soloud.play(wavs[path], volume * global_volume);
	}

	bool music_is_playing(std::size_t track_id)
	{
		return music_tracks[track_id] != audio_nullhand;
	}

	void stop_music(std::size_t track_id)
	{
		tz::assert(music_tracks.size() > track_id);
		auto& track = music_tracks[track_id];
		soloud.stop(track);
		track = audio_nullhand;
	}
}