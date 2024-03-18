#ifndef REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#include "gamelib/scene.hpp"
#include "tz/core/messaging.hpp"
#include "tz/lua/api.hpp"
#include <any>

namespace game::messaging
{
	// what should rn.current_scene() refer to?
	void set_current_scene(game::scene& scene);
	// initialise lua api for passing messages. call on main thread.
	void scene_messaging_lua_initialise(tz::lua::state& state);
	// carry out all messages received from local dispatches. call on main thread.
	void scene_messaging_update();
	// send all local messages from lua advance to the global message receiver. call on any thread.
	void scene_messaging_local_dispatch();
	// force all job worker threads to do a local dispatch and block the current thread till they're all done. call on any thread (you probably want main thread tho)
	// this is poor performance and you should avoid caLLing this on the hot path. just make sure all job workers that run lua invoke their local_dispatch above before scene_messaging_update is invoked and then you can forget this function exists.
	void scene_messaging_force_dispatches();

	enum class scene_operation
	{
		add_entity,
		remove_entity,
		clear_entities,
		entity_write,
		entity_write_add,
		entity_write_multiply,
		entity_write_multiply_percentage,
		entity_set_name,
		entity_set_local_position,
		entity_set_local_rotation,
		entity_set_local_scale,
		entity_set_global_position,
		entity_set_global_rotation,
		entity_set_global_scale,
		entity_play_animation,
		entity_queue_animation,
		entity_set_subobject_texture_name,
		entity_set_subobject_texture_colour,
		entity_set_subobject_visible,
		entity_set_subobject_pixelated,
		renderer_set_camera_position,
		renderer_set_clear_colour,
		renderer_set_ambient_light,
		renderer_add_texture,
		renderer_add_model,
		renderer_add_light,
		renderer_remove_light,
		renderer_light_set_position,
		renderer_light_set_colour,
		renderer_light_set_power,
		renderer_clear_lights,
		renderer_directional_light_set_direction,
		renderer_directional_light_set_power,
		renderer_directional_light_set_colour,
		renderer_add_string,
		renderer_remove_string,
		renderer_clear_strings,
		renderer_string_set_position,
		audio_play_sound,
		audio_play_music,
		audio_stop_music
	};

	struct scene_message
	{
		scene_operation operation;
		entity_uuid uuid;
		std::any value = {};

		#if TZ_DEBUG
		std::string debug_source_location = tz::lua::get_state().print_traceback();
		#endif
	};

	entity_uuid scene_quick_add(std::string prefab_name);
	void scene_insert_message(scene_message msg);
}

#endif // REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
