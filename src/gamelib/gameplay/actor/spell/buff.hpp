#ifndef RNLIB_GAMEPLAY_ACTOR_SPELL_BUFF_HPP
#define RNLIB_GAMEPLAY_ACTOR_SPELL_BUFF_HPP
#include "tz/core/time.hpp"
#include <optional>

namespace rnlib
{
	enum class buff_id
	{
		
	};

	enum class buff_type
	{
		timed,
		environmental
	};

	struct buff
	{
		buff_id id;
		buff_type type;
		std::optional<float> duration_remaining = std::nullopt;
		const char* name = "";
	};

	buff create_buff(buff_id id);
}

#endif // RNLIB_GAMEPLAY_ACTOR_SPELL_BUFF_HPP
