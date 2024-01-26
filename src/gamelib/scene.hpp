#ifndef REDNIGHTMARE_GAMELIB_SCENE_HPP
#define REDNIGHTMARE_GAMELIB_SCENE_HPP
#include "gamelib/entity.hpp"
#include "tz/core/data/free_list.hpp"
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

		entity_handle add_entity(std::uint_fast64_t uuid);
		void remove_entity(entity_handle e);
		void remove_entity(std::uint_fast64_t uuid);
		void clear();
	private:
		// free list gives handle stability, which we want.
		// hashmap gives fast lookup for those who want to index by uuid (which everyone will want to do)
		tz::free_list<scene_entity_data> entities = {};
		std::unordered_map<std::uint_fast64_t, entity_handle> uuid_entity_map = {};
	};
}

#endif // REDNIGHTMARE_GAMELIB_SCENE_HPP
