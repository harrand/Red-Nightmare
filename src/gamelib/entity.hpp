#ifndef REDNIGHTMARE_GAMELIB_ENTITY_HPP
#define REDNIGHTMARE_GAMELIB_ENTITY_HPP
#include <limits>
#include <cstdint>

namespace game
{
	constexpr std::uint_fast64_t null_uuid = std::numeric_limits<std::uint_fast64_t>::max();

	struct entity
	{
		std::uint_fast64_t uuid;

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
