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
		entity_write,
		entity_set_name,
		entity_set_local_position,
		entity_set_local_scale,
		entity_set_global_position,
		entity_set_global_scale,
	};

	struct scene_message
	{
		scene_operation operation;
		entity_uuid uuid;
		std::any value = {};
	};
}

#endif // REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
