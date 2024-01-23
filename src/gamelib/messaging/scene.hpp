#ifndef REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
#include "tz/core/messaging.hpp"
#include "tz/lua/api.hpp"

namespace game::messaging
{
	void scene_messaging_lua_initialise();
	void scene_messaging_update();

	enum class scene_operation
	{
		add_entity
	};

	struct scene_message
	{
		scene_operation operation;
		std::uint64_t uuid;
	};
}

#endif // REDNIGHTMARE_GAMELIB_MESSAGING_SCENE_HPP
