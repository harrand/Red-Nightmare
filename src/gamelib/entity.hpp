#ifndef REDNIGHTMARE_GAMELIB_ENTITY_HPP
#define REDNIGHTMARE_GAMELIB_ENTITY_HPP
#include <limits>
#include <cstdint>

namespace game
{
	using entity_uuid = std::uint_fast64_t;
	constexpr entity_uuid null_uuid = std::numeric_limits<entity_uuid>::max();

	struct entity
	{
		entity_uuid uuid;

		static entity null()
		{
			return {.uuid = null_uuid};
		}

		bool is_null() const
		{
			return this->uuid == null_uuid;
		}
	};
}

#endif // REDNIGHTMARE_GAMELIB_ENTITY_HPP
