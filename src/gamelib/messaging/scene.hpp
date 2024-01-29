#ifndef REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#include "gamelib/scene.hpp"
#include "tz/core/messaging.hpp"
#include "tz/lua/api.hpp"
#include <any>

namespace game::messaging
{
	void scene_messaging_lua_initialise();
	void scene_messaging_update(game::scene& scene);

	enum class scene_operation
	{
		add_entity,
		remove_entity,
		entity_write,
		entity_set_name,
	};

	struct scene_message
	{
		scene_operation operation;
		entity_uuid uuid;
		std::any value = {};
	};
}

#endif // REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
