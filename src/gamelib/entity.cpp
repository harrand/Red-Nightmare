#include "gamelib/entity.hpp"
#include "tz/core/imported_text.hpp"

#include ImportedTextHeader(entity, lua)

namespace game
{
	void entity_lua_initialise()
	{
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			std::string entity_lua_src{ImportedTextData(entity, lua)};
			state.execute(entity_lua_src.c_str());
		});
	}
}
