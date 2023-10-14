#include "gamelib/entity/scene.hpp"
#include "tz/core/imported_text.hpp"

#include ImportedTextHeader(entity, lua)

namespace game::entity
{
	game::physics::aabb scene_quadtree_node::get_aabb() const
	{
		entity& e = this->sc->get(this->entity_hanval);
		auto& elem = e.elem;
		// get first object, represent its scale and position as a very basic aabb

		auto& ren = elem.renderer->get_renderer();
		tz::trs transform = ren.get_object_base_transform(elem.entry.pkg.objects.front());
		tz::vec2 position = transform.translate.swizzle<0, 1>();
		tz::vec2 half_scale = transform.scale.swizzle<0, 1>() * 0.5f;
		if(elem.get_model() == game::render::scene_renderer::model::humanoid)
		{
			// humanoid has implicit internal scale of 0.001. multiply by 1000 to offset
			half_scale *= 1000.0f;
		}
		// min is position - half_scale
		// max is position + half_scale
		return {position - half_scale, position + half_scale};
	}

	render::scene_renderer& scene::get_renderer()
	{
		return this->renderer;
	}

	std::size_t scene::debug_get_intersection_count() const
	{
		return this->intersection_state.size();
	}

	scene::entity_handle scene::add(std::size_t type)
	{
		TZ_PROFZONE("scene - add", 0xFF99CC44);
		tz::hanval ret_hanval;
		if(this->free_list.empty())
		{
			this->entities.push_back({.type = type});
			ret_hanval = static_cast<tz::hanval>(this->entities.size() - 1);
		}	
		else
		{
			ret_hanval = static_cast<tz::hanval>(this->free_list.front());
			this->entities[static_cast<std::size_t>(ret_hanval)] = {.type = type};
			this->free_list.pop_front();
		}
		this->initialise_entity(ret_hanval, type);
		return ret_hanval;
	}

	void scene::remove(entity_handle e)
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(e));
		if(std::find(this->free_list.begin(), this->free_list.end(), e) != this->free_list.end())
		{
			// double remove. is bad!
			tz::report("scene::remove(%zu) - double remove detected!", hanval);
			return;
		}
		this->free_list.push_back(e);
		this->renderer.remove_model(this->entities[hanval].elem.entry);
		this->deinitialise_entity(static_cast<tz::hanval>(hanval), this->entities[hanval].uid);
		this->entities[hanval] = {};
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

	std::size_t scene::size() const
	{
		return this->entities.size();
	}

	void scene::update(float delta_seconds)
	{
		this->renderer.update(delta_seconds);
		this->rebuild_quadtree();
	}

	void scene::lua_initialise(tz::lua::state& state)
	{
		TZ_PROFZONE("scene - lua initialise", 0xFF99CC44);
		this->get_renderer().lua_initialise(state);
		state.new_type("rn_impl_entity", LUA_CLASS_NAME(rn_impl_entity)::registers);
		state.new_type("rn_impl_scene", LUA_CLASS_NAME(rn_impl_scene)::registers);
		
		std::string str{ImportedTextData(entity, lua)};
		state.execute(str.c_str());
	}

	void scene::initialise_entity(tz::hanval entity_hanval, std::size_t type)
	{
		TZ_PROFZONE("scene - initialise entity", 0xFF99CC44);
		auto& state = tz::lua::get_state();
		rn_impl_entity lua_data{.scene = this, .entity_hanval = entity_hanval};
		LUA_CLASS_PUSH(state, rn_impl_entity, lua_data);
		state.assign_stack("rn_impl_new_entity");
		std::string cmd = "rn.entity_preinit(" + std::to_string(type) + ")";
		state.execute(cmd.c_str());

		lua_data.get().elem = this->get_renderer().get_element(this->get_renderer().add_model(lua_data.get().elem.entry.m));
		cmd = "rn.entity_postinit(" + std::to_string(type) + ")";
		state.execute(cmd.c_str());
	}

	void scene::deinitialise_entity(tz::hanval entity_hanval, std::size_t uid)
	{
		auto& state = tz::lua::get_state();
		rn_impl_entity lua_data{.scene = this, .entity_hanval = entity_hanval};
		auto& ent = this->get(entity_hanval);
		LUA_CLASS_PUSH(state, rn_impl_entity, lua_data);
		state.assign_stack("rn_impl_dead_entity");
		std::string cmd = "rn.entity_deinit()";
		state.execute(cmd.c_str());
	}

	void scene::rebuild_quadtree()
	{
		this->quadtree.clear();
		for(std::size_t i = 0; i < this->size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			if(this->is_valid(hanval))
			{
				this->quadtree.add({.sc = this, .entity_hanval = hanval});
			}
		}
		this->intersection_state = this->quadtree.find_all_intersections();
	}

	bool scene::is_valid(tz::hanval entity_hanval) const
	{
		return std::find(this->free_list.begin(), this->free_list.end(), entity_hanval) == this->free_list.end();
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

	int rn_impl_scene::remove_uid(tz::lua::state& state)
	{
		auto [_, uid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		for(std::size_t i = 0; i < this->sc->size(); i++)
		{
			auto hv = static_cast<tz::hanval>(i);
			if(this->sc->get(hv).uid == uid)
			{
				this->sc->remove(hv);
				break;
			}
		}
		return 0;
	}

	int rn_impl_scene::get(tz::lua::state& state)
	{
		auto [_, eh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		rn_impl_entity ent{.scene = this->sc, .entity_hanval = static_cast<tz::hanval>(eh)};
		LUA_CLASS_PUSH(state, rn_impl_entity, ent);
		return 1;
	}

	int rn_impl_scene::get_renderer(tz::lua::state& state)
	{
		using namespace game::render;
		LUA_CLASS_PUSH(state, impl_rn_scene_renderer, {.renderer = &this->sc->get_renderer()});
		return 1;
	}

	int rn_impl_scene::size(tz::lua::state& state)
	{
		state.stack_push_uint(this->sc->size());
		return 1;
	}
}