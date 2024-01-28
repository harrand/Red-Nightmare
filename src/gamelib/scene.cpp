#include "gamelib/scene.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"

namespace game
{
	scene::entity_handle scene::add_entity(entity_uuid uuid)
	{
		TZ_PROFZONE("scene - add entity", 0xFF99CC44);
		// add to entity list.
		entity_handle ret = this->entities.push_back({.ent = {.uuid = uuid}});
		// map uuid to entity handle (for fast lookup times)
		this->uuid_entity_map[uuid] = ret;
		
		std::string preinit_lua;
		preinit_lua += "rn.entity.preinit(" + std::to_string(uuid) + ")";
		tz::lua::get_state().execute(preinit_lua.c_str());
		// initialise scene element. model etc has been chosen by now.
		
		std::string init_lua;
		init_lua += "rn.entity.init(" + std::to_string(uuid) + ")";
		tz::lua::get_state().execute(init_lua.c_str());
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
