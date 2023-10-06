#include "gamelib/entity/api.hpp"
#include "gamelib/entity/scene.hpp"

namespace game::entity
{
	entity& rn_impl_entity::get()
	{
		return this->scene->get(this->entity_hanval);
	}

	int rn_impl_entity::get_name(tz::lua::state& state)
	{
		state.stack_push_string(this->get().name);
		return 1;
	}
}