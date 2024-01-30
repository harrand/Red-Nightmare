#include "gamelib/scene.hpp"
#include "gamelib/messaging/scene.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include <format>

namespace game
{
	scene::entity_handle scene::add_entity(entity_uuid uuid)
	{
		TZ_PROFZONE("scene - add entity", 0xFF99CC44);
		// add to entity list.
		entity_handle ret = this->entities.push_back({.ent = {.uuid = uuid}});
		// map uuid to entity handle (for fast lookup times)
		this->uuid_entity_map[uuid] = ret;

		// todo: associate with a single empty object in an animation_renderer. as it needs to be a part of the transform hierarchy.
		return ret;
	}

	scene::entity_handle scene::add_entity_from_prefab(entity_uuid uuid, const std::string& prefab_name)
	{
		TZ_PROFZONE("scene - add entity from prefab", 0xFF99CC44);

		std::string preinit_lua;
		preinit_lua += "rn.entity.pre_instantiate(" + std::to_string(uuid) + ", \"" + prefab_name + "\")";
		tz::lua::get_state().execute(preinit_lua.c_str());
		// initialise scene element. model etc has been chosen by now.
		
		std::string init_lua;
		init_lua += "rn.entity.instantiate(" + std::to_string(uuid) + ", \"" + prefab_name + "\")";
		tz::lua::get_state().execute(init_lua.c_str());
		return this->add_entity(uuid);
	}

	scene::entity_handle scene::add_entity_from_existing(entity_uuid uuid, entity_uuid existing)
	{
		TZ_PROFZONE("scene - add entity from existing", 0xFF99CC44);
		// add to entity list.
		entity_handle ret = this->add_entity(uuid);
		auto& ent = this->entities[ret].ent;
		ent = this->get_entity(existing);
		ent.internal_variables.clear();
		return ret;
	}

	void scene::remove_entity(entity_handle e)
	{
		TZ_PROFZONE("scene - remove entity", 0xFF99CC44);
		auto uuid = this->entities[e].ent.uuid;
		this->entities.erase(e);
		this->uuid_entity_map.erase(uuid);
	}

	void scene::remove_entity(entity_uuid uuid)
	{
		// find entity handle using uuid,
		// then remove by entity handle.
		TZ_PROFZONE("scene - remove entity (find uuid)", 0xFF99CC44);
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		this->remove_entity(iter->second);
	}

	void scene::clear()
	{
		this->entities.clear();
		this->uuid_entity_map.clear();
	}

	constexpr std::size_t single_threaded_update_limit = 128u;

	void scene::update(float delta_seconds)
	{
		TZ_PROFZONE("scene - update", 0xFFCCAACC);

		auto count = this->entity_count();

		std::size_t job_count = tz::job_system().worker_count();
		std::size_t objects_per_job = count / job_count;

		if(count == 0)
		{
			return;
		}
		if(count <= single_threaded_update_limit || objects_per_job == 0 || job_count == 1)
		{
			// do a single threaded update. poor use of CPU resources, but means no latency sending thread-local messages. this means very low cpu usage (or massive fps number if uncapped)
			std::string lua_str;
			for(const scene_entity_data& ed : this->entities)
			{
				lua_str += std::format("rn.entity.update({})", ed.ent.uuid);
			}
			tz::lua::get_state().execute(lua_str.c_str());
			game::messaging::scene_messaging_local_dispatch();
			return;
		}

		// do a multi-threaded update.
		std::size_t remainder_objects = count % job_count;
		tz::assert((objects_per_job * job_count) + remainder_objects == count);
		this->entity_update_jobs.resize(job_count);
		for(std::size_t i = 0; i < job_count; i++)
		{
			this->entity_update_jobs[i] = tz::job_system().execute([this, delta_seconds, offset = i * objects_per_job, object_count = objects_per_job]
			{
				TZ_PROFZONE("scene - update some", 0xFFCCAACC);
				std::string lua_str;
				lua_str.reserve(sizeof("rn.entity.update(XXXX)") * object_count);
				auto begin = std::next(this->uuid_entity_map.begin(), offset);
				for(std::size_t j = 0; j < object_count; j++)
				{
					lua_str += std::format("rn.entity.update({})", this->entities[(begin++)->second].ent.uuid);
				}
				// execute update on all entities
				tz::lua::get_state().execute(lua_str.c_str());
				// those entity updates probably generated a bunch of messages.
				// as the global message receiver is completely thread-safe, we can concurrently send them over with other threads.
				// there will be some lock contention, but very minor - as there is a single lock per worker, no matter how many messages it generates.
				game::messaging::scene_messaging_local_dispatch();
			});
		}
		
	}

	void scene::fixed_update(float delta_seconds, std::uint64_t unprocessed)
	{

	}

	void scene::block()
	{
		for(tz::job_handle jh : this->entity_update_jobs)
		{
			tz::job_system().block(jh);
		}
		this->entity_update_jobs.clear();
	}

	std::size_t scene::entity_count() const
	{
		return this->entities.size();
	}

	const game::entity& scene::get_entity(entity_handle e) const
	{
		return this->entities[e].ent;
	}

	game::entity& scene::get_entity(entity_handle e)
	{
		return this->entities[e].ent;
	}

	const game::entity& scene::get_entity(entity_uuid uuid) const
	{
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		return this->get_entity(iter->second);
	}

	game::entity& scene::get_entity(entity_uuid uuid)
	{
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		return this->get_entity(iter->second);
	}
}
