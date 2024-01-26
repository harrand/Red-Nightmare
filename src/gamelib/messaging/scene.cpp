#include "gamelib/messaging/scene.hpp"
#include "gamelib/messaging/system.hpp"
#include "tz/core/profile.hpp"

#include <iostream>

namespace game::messaging
{
	game::scene* sc = nullptr;

	void on_scene_process_message(const scene_message& msg)
	{
		std::cout << "scene mc message detected: " << static_cast<int>(msg.operation) << "\n";
		tz::assert(sc != nullptr);
		switch(msg.operation)
		{
			case scene_operation::add_entity:
				sc->add_entity(msg.uuid);	
			break;
			case scene_operation::remove_entity:
				sc->remove_entity(msg.uuid);
			break;
		}
	}

	REGISTER_MESSAGING_SYSTEM(scene_message, scene, on_scene_process_message);

	// lua api boilerplate.
	// yes, this is rn.current_scene()

	struct lua_local_scene_message_receiver
	{
		int add_entity(tz::lua::state& state)
		{
			// entity uuid is created *now* and instantly returned.
			// caller now knows the entity id, even though it doesnt exist yet.
			// subsequent messages that use the id *should* be processed after this one, making the whole thing safe.
			static std::atomic_uint_fast64_t entity_uuid_counter = 0;
			std::uint_fast64_t entity_id = entity_uuid_counter.fetch_add(1);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::add_entity,
				// add new entity. its uuid is `entity_id`
				.uuid = entity_id
			});
			// return the id as a uint.
			// TODO: wrap userdata around this?
			state.stack_push_uint(entity_id);
			return 1;
		}

		int remove_entity(tz::lua::state& state)
		{
			auto [_, entity_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::remove_entity,
				.uuid = entity_id
			});
			return 0;
		}
	};

	LUA_CLASS_BEGIN(lua_local_scene_message_receiver)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(lua_local_scene_message_receiver, add_entity)
			LUA_METHOD(lua_local_scene_message_receiver, remove_entity)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(rn_current_scene)
		LUA_CLASS_PUSH(state, lua_local_scene_message_receiver, lua_local_scene_message_receiver{});
		return 1;
	LUA_END

	// implementation.

	void scene_messaging_lua_initialise()
	{
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			// set the local message passer to target the global receiver. otherwise all its messages will be dropped.
			local_scene_receiver.set_target(global_scene_receiver);
			// expose the class api
			state.execute("rn = rn or {}");
			state.new_type("lua_local_scene_message_receiver", LUA_CLASS_NAME(lua_local_scene_message_receiver)::registers);
			state.assign_func("rn.current_scene", LUA_FN_NAME(rn_current_scene));
			// rn.current_scene:add_entity(...) is now a thing.
		});
	}

	void scene_messaging_update(game::scene& scene)
	{
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			TZ_PROFZONE("scene - pass local messages", 0xFF99CC44);
			// for each state, invoke update() on its local scene receiver.
			// this passes all its messages to the global scene receiver.
			local_scene_receiver.update();
		});

		// global receiver will now have all the messages ready.
		// we can finally process them all.
		TZ_PROFZONE("scene - process all messages", 0xFF99CC44);
		sc = &scene;
		global_scene_receiver.update();
	}
}
