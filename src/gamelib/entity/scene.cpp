#include "gamelib/entity/scene.hpp"
#include "tz/core/imported_text.hpp"

#include ImportedTextHeader(entity, lua)

namespace game::entity
{
	render::scene_renderer& scene::get_renderer()
	{
		return this->renderer;
	}

	scene::entity_handle scene::add(std::size_t type)
	{
		tz::hanval ret_hanval;
		if(this->free_list.empty())
		{
			this->entities.push_back({});
			ret_hanval = static_cast<tz::hanval>(this->entities.size() - 1);
		}	
		this->initialise_entity(ret_hanval, type);
		return ret_hanval;
	}

	void scene::remove(entity_handle e)
	{

	}

	const entity& scene::get(entity_handle e) const
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(e));
		tz::assert(hanval < this->entities.size());
		return this->entities[hanval];
	}

	entity& scene::get(entity_handle e)
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(e));
		tz::assert(hanval < this->entities.size());
		return this->entities[hanval];
	}

	void scene::lua_initialise(tz::lua::state& state)
	{
		this->get_renderer().lua_initialise(state);
		state.new_type("rn_impl_entity", LUA_CLASS_NAME(rn_impl_entity)::registers);
		state.new_type("rn_impl_scene", LUA_CLASS_NAME(rn_impl_scene)::registers);
		
		std::string str{ImportedTextData(entity, lua)};
		state.execute(str.c_str());
	}

	void scene::initialise_entity(tz::hanval entity_hanval, std::size_t type)
	{
		auto& state = tz::lua::get_state();
		rn_impl_entity lua_data{.scene = this, .entity_hanval = entity_hanval};
		LUA_CLASS_PUSH(state, rn_impl_entity, lua_data);
		state.assign_stack("rn_impl_new_entity");
		std::string cmd = "rn.initialise_entity(" + std::to_string(type) + ")";
		state.execute(cmd.c_str());

		lua_data.get().elem = this->get_renderer().get_element(this->get_renderer().add_model(lua_data.get().elem.entry.m));
	}

	// LUA API

	int rn_impl_scene::add(tz::lua::state& state)
	{
		auto [_, type] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		scene::entity_handle e = this->sc->add(type);
		state.stack_push_uint(static_cast<std::size_t>(static_cast<tz::hanval>(e)));
		return 1;
	}

	int rn_impl_scene::remove(tz::lua::state& state)
	{
		auto [_, eh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		this->sc->remove(static_cast<tz::hanval>(eh));
		return 0;
	}

	int rn_impl_scene::get(tz::lua::state& state)
	{
		auto [_, eh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		rn_impl_entity ent{.scene = this->sc, .entity_hanval = static_cast<tz::hanval>(eh)};
		LUA_CLASS_PUSH(state, rn_impl_entity, ent);
		return 1;
	}
}