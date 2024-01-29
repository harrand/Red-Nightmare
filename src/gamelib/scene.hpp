#ifndef REDNIGHTMARE_GAMELIB_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_SCENE_HPP
#include "gamelib/entity.hpp"
#include "tz/core/data/free_list.hpp"
#include "tz/core/job/job.hpp"
#include <unordered_map>

namespace game
{
	struct scene_entity_data
	{
		game::entity ent;

		static scene_entity_data null()
		{
			return {.ent = game::entity::null()};
		}

		bool is_null() const
		{
			return this->ent.is_null();
		}
	};

	class scene
	{
	public:
		scene() = default;
		using entity_handle = tz::free_list<scene_entity_data>::handle;

		entity_handle add_entity(entity_uuid uuid);
		entity_handle add_entity_from_prefab(entity_uuid uuid, const std::string& prefab_name);
		entity_handle add_entity_from_existing(entity_uuid uuid, entity_uuid existing);
		void remove_entity(entity_handle e);
		void remove_entity(entity_uuid uuid);
		void clear();
		void update(float delta_seconds);
		void fixed_update(float delta_seconds, std::uint64_t unprocessed);
		void block();

		std::size_t entity_count() const;
		const game::entity& get_entity(entity_handle e) const;
		game::entity& get_entity(entity_handle e);
		const game::entity& get_entity(entity_uuid uuid) const;
		game::entity& get_entity(entity_uuid uuid);
	private:
		// free list gives handle stability, which we want.
		// hashmap gives fast lookup for those who want to index by uuid (which everyone will want to do)
		tz::free_list<scene_entity_data> entities = {};
		std::unordered_map<entity_uuid, entity_handle> uuid_entity_map = {};
		std::vector<tz::job_handle> entity_update_jobs = {};
	};
}

#endif // REDNIGHTMARE_GAMELIB_SCENE_HPP
