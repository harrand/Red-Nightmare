#include "gamelib/messaging/scene.hpp"
#include "gamelib/messaging/system.hpp"
#include "tz/core/profile.hpp"

#include <iostream>

namespace game::messaging
{
	// right so how does all this shit work?
	// each job worker (equal to number of hardware threads) has its own thread-local message passer.
	// whenever lua code wants to perform some form of write/mutation (e.g adding a new entity, changing an internal variable) it sends a message to its thread-local message passer.
	// at the end of the update cycle, these all sync up - all the thread-local passers redirect their messages to one global receiver. all the messages are then processed via `on_scene_process_message`. Because these are all collaborated and done on a single thread, we avoid false sharing issues and dont have to lock anything.
	// this has an additional advantage:
	// whenever lua code wants to perform some kind of read operation, it does not send a message - it simply instantly does the read without any kind of lock. X threads can concurrently read from a variable so long as no write operations happen. we can guarantee this because those are all deferred to the end of the update.
	// this has one drawback: reads technically read from the previous-frame's state, and writes do not take affect until the end of the frame.
	// however, with clever use of the thread-local message passers, we could in theory check past writes when figuring out what to return for a read (this part is NYI).



	game::scene* sc = nullptr;

	void on_scene_process_message(const scene_message& msg)
	{
		TZ_PROFZONE("scene message", 0xFF99CC44);
		tz::assert(sc != nullptr);
		switch(msg.operation)
		{
			case scene_operation::add_entity:
			{
				auto val = std::any_cast<tz::lua::lua_generic>(msg.value);
				std::visit([&msg](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					if constexpr(std::is_same_v<T, std::string>) // add_entity(prefab_name) (instantiate from prefab)
					{
						tz::report("init from prefab \"%s\"", arg.c_str());
						sc->add_entity_from_prefab(msg.uuid, arg);
					}
					else if constexpr(std::is_same_v<T, std::int64_t>) // add entity(uuid) (entity copy)
					{
						tz::report("init from uuid %llu", arg);
						sc->add_entity_from_existing(msg.uuid, arg);
					}
					else if constexpr(std::is_same_v<T, tz::lua::nil>) // add entity() (empty entity)
					{
						sc->add_entity(msg.uuid);
					}
					else // some cursed shit.
					{
						tz::error("add_entity(arg) invoked with invalid argument type. Argument must be string (prefab name) or int (uuid)");
					}
				}, val);
			}
			break;
			case scene_operation::remove_entity:
				sc->remove_entity(msg.uuid);
			break;
			case scene_operation::entity_write:
			{
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				sc->get_entity(msg.uuid).internal_variables[varname] = lua_value;
			}
			break;
			case scene_operation::entity_set_name:
				sc->get_entity(msg.uuid).name = std::any_cast<std::string>(msg.value);
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
			TZ_PROFZONE("scene - add entity", 0xFF99CC44);
			tz::lua::lua_generic arg = tz::lua::nil{};
			if(state.stack_size() > 1)
			{
				arg = state.stack_get_generic(2);
			}
			// entity uuid is created *now* and instantly returned.
			// caller now knows the entity id, even though it doesnt exist yet.
			// subsequent messages that use the id *should* be processed after this one, making the whole thing safe.
			static std::atomic_uint_fast64_t entity_uuid_counter = 0;
			auto entity_id = static_cast<entity_uuid>(entity_uuid_counter.fetch_add(1));
			local_scene_receiver.send_message
			({
				.operation = scene_operation::add_entity,
				// add new entity. its uuid is `entity_id`
				.uuid = entity_id,
				.value = arg
			});
			// return the id as a uint.
			// TODO: wrap userdata around this?
			state.stack_push_uint(entity_id);
			return 1;
		}

		int remove_entity(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - remove entity", 0xFF99CC44);
			auto [_, entity_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::remove_entity,
				.uuid = static_cast<entity_uuid>(entity_id)
			});
			return 0;
		}

		int entity_write(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity write", 0xFF99CC44);
			auto [_, entity_id, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			tz::lua::lua_generic variable_value = state.stack_get_generic(4);
			std::pair<std::string, tz::lua::lua_generic> message{varname, variable_value};
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_write,
				.uuid = static_cast<entity_uuid>(entity_id),
				.value = message
			});
			return 0;
		}

		int entity_read(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity read", 0xFF99CC44);
			// reads instantly.
			auto [_, entity_id, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			const auto& vars = sc->get_entity(entity_id).internal_variables;
			// note: this code could be ran concurrently.
			// because all forms of scene/entity mutation are deferred, reading concurrently is safe here. note: the map operator[] accidentally constructing empties would be a data race here, so we are careful and use find() instead.
			auto iter = vars.find(varname);
			tz::lua::lua_generic ret = tz::lua::nil{};
			if(iter != vars.end())
			{
				ret = iter->second;
			}
			state.stack_push_generic(ret);
			return 1;
		}

		int entity_set_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set name", 0xFF99CC44);
			auto [_, entity_id, name] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_name,
				.uuid = static_cast<entity_uuid>(entity_id),
				.value = name
			});
			return 0;
		}

		int entity_get_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get name", 0xFF99CC44);
			auto [_, entity_id] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			state.stack_push_string(sc->get_entity(entity_id).name);	
			return 1;
		}
	};

	LUA_CLASS_BEGIN(lua_local_scene_message_receiver)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(lua_local_scene_message_receiver, add_entity)
			LUA_METHOD(lua_local_scene_message_receiver, remove_entity)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write)
			LUA_METHOD(lua_local_scene_message_receiver, entity_read)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_name)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_name)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(rn_current_scene)
		LUA_CLASS_PUSH(state, lua_local_scene_message_receiver, lua_local_scene_message_receiver{});
		return 1;
	LUA_END

	// implementation.

	void scene_messaging_lua_initialise(game::scene& scene)
	{
		sc = &scene;
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
		TZ_PROFZONE("scene - update messages", 0xFFAAAACC);
		sc = &scene;
		// if a multi-threaded update was done, tell all the threads to stop and collaborate.
		if(scene.needs_block())
		{
			tz::lua::for_all_states([](tz::lua::state& state)
			{
				TZ_PROFZONE("scene - pass local messages", 0xFF99CC44);
				// for each state, invoke update() on its local scene receiver.
				// this passes all its messages to the global scene receiver.
				local_scene_receiver.update();
			}, false);
		}
		// if not... well only this main thread's local receiver needs to pass its message. no jobs needed.
		else
		{
			local_scene_receiver.update();
		}

		// global receiver will now have all the messages ready.
		// we can finally process them all.
		TZ_PROFZONE("scene - process all messages", 0xFF99CC44);
		global_scene_receiver.update();
	}
}
