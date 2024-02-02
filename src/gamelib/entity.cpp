#include "gamelib/entity.hpp"
#include "tz/core/imported_text.hpp"

#include ImportedTextHeader(entity, lua)

namespace game
{
	/*static*/ entity entity::null()
	{
		return {.uuid = null_uuid};
	}

	bool entity::is_null() const
	{
		return this->uuid == null_uuid;
	}

	void entity_lua_initialise(tz::lua::state& state)
	{
		std::string entity_lua_src{ImportedTextData(entity, lua)};
		state.execute(entity_lua_src.c_str());
	}
}
