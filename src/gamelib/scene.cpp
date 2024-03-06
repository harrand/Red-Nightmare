#include "gamelib/scene.hpp"
#include "gamelib/lua/meta.hpp"
#include "gamelib/messaging/scene.hpp"
#include "gamelib/physics/grid_hierarchy.hpp"
#include "gamelib/render/scene_renderer.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"
#include "tz/lua/state.hpp"
#include "tz/wsi/monitor.hpp"
#include <format>
#include <variant>

namespace game
{
	scene::scene():
	grid(tz::vec2::zero(), this->get_renderer().get_view_bounds() * 8.0f, static_cast<physics::grid_hierarchy::cell_coord>(tz::vec2i{50, 50}))
	{
	}

	scene::entity_handle scene::add_entity(entity_uuid uuid)
	{
		TZ_PROFZONE("scene - add entity", 0xFF99CC44);
		// add to entity list.
		entity_handle ret = this->entities.push_back({.ent = {.uuid = uuid}});
		// map uuid to entity handle (for fast lookup times)
		this->uuid_entity_map[uuid] = ret;

		//auto entry = this->renderer.add_model(game::render::scene_renderer::model::humanoid);
		//this->renderer.get_element(entry).play_animation_by_name("Run", true);

		// todo: associate with a single empty object in an animation_renderer. as it needs to be a part of the transform hierarchy.
		return ret;
	}

	scene::entity_handle scene::add_entity_from_prefab(entity_uuid uuid, const std::string& prefab_name)
	{
		TZ_PROFZONE("scene - add entity from prefab", 0xFF99CC44);
		entity_handle ret = this->add_entity(uuid);

		std::string preinit_lua = std::format(R"(
			last_model = rn.entity.pre_instantiate({}, "{}")
			)", uuid, prefab_name);
		tz::lua::get_state().execute(preinit_lua.c_str());
		std::string model_name = tz::lua::get_state().get_string("last_model").value_or("");

		auto& ent = this->entities[ret];
		ent.ent.set_internal(".prefab", prefab_name);
		// initialise scene element, if a model was selected.
		if(model_name.size())
		{
			ent.ren.model_name = model_name;

			this->initialise_renderer_component(uuid);
			this->notify_new_entity(uuid);
		}
		
		std::string init_lua = std::format("rn.entity.instantiate({}, \"{}\")", uuid, prefab_name);
		tz::lua::get_state().execute(init_lua.c_str());
		return ret;
	}

	scene::entity_handle scene::add_entity_from_existing(entity_uuid uuid, entity_uuid existing)
	{
		TZ_PROFZONE("scene - add entity from existing", 0xFF99CC44);
		tz::assert(this->contains_entity(existing), "Attempt to create entity from existing copy %lu, but this entity does not exist.", existing);
		// add to entity list.
		entity_handle ret = this->add_entity(uuid);
		auto& existing_ent = this->entities[this->uuid_entity_map[existing]];

		auto& ent = this->entities[ret];
		ent.ent = this->get_entity(existing);
		ent.ren.model_name = existing_ent.ren.model_name;
		std::erase_if(ent.ent.internal_variables, [](const auto& iter)
		{
			const auto& [varname, value] = iter;
			return !varname.starts_with('.');
		});

		if(!ent.ren.model_name.empty())
		{
			this->initialise_renderer_component(uuid);
			this->notify_new_entity(uuid);
		}
		return ret;
	}

	void scene::remove_entity(entity_handle e)
	{
		TZ_PROFZONE("scene - remove entity", 0xFF99CC44);
		const auto& ent = this->entities[e];

		// if it has renderer component - it needs to be destroyed.
		auto uuid = ent.ent.uuid;
		if(ent.ren.obj != tz::nullhand)
		{
			this->renderer.remove_entry(ent.ren);
			this->grid.remove_entity(uuid);
		}

		this->entities.erase(e);
		this->uuid_entity_map.erase(uuid);
	}

	void scene::remove_entity(entity_uuid uuid)
	{
		// find entity handle using uuid,
		// then remove by entity handle.
		TZ_PROFZONE("scene - remove entity (find uuid)", 0xFF99CC44);
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		this->remove_entity(iter->second);
	}

	void scene::clear()
	{
		this->entities.clear();
		this->uuid_entity_map.clear();
		this->renderer.clear_entries();
		this->grid.clear();
	}

	constexpr std::size_t single_threaded_update_limit = 32u;

	void scene::update(float delta_seconds)
	{
		TZ_PROFZONE("scene - update", 0xFFCCAACC);
		this->mouse_pos_ws = this->calc_mouse_position_world_space();
		this->renderer.update(delta_seconds);	

		auto count = this->entity_count();

		std::size_t job_count = tz::job_system().worker_count();
		std::size_t objects_per_job = count / job_count;

		if(count == 0)
		{
			return;
		}
		if(count <= single_threaded_update_limit || objects_per_job == 0 || job_count == 1)
		{
			// do a single threaded update. poor use of CPU resources, but means no latency sending thread-local messages. this means very low cpu usage (or massive fps number if uncapped)
			std::string lua_str;
			for(const scene_entity_data& ed : this->entities)
			{
				lua_str += std::format("rn.entity.update({}, {})", ed.ent.uuid, delta_seconds);
			}
			tz::lua::get_state().execute(lua_str.c_str());
			game::messaging::scene_messaging_local_dispatch();
			return;
		}

		// do a multi-threaded update.
		std::size_t remainder_objects = count % job_count;
		tz::assert((objects_per_job * job_count) + remainder_objects == count);
		this->entity_update_jobs.resize(job_count);
		for(std::size_t i = 0; i < job_count; i++)
		{
			this->entity_update_jobs[i] = tz::job_system().execute([this, delta_seconds, offset = i * objects_per_job, object_count = objects_per_job]
			{
				TZ_PROFZONE("scene - update some", 0xFFCCAACC);
				std::string lua_str;
				lua_str.reserve(sizeof("rn.entity.update(XXXXXXXXXXXX)") * object_count);
				// update delta-time while we're here.
				auto begin = std::next(this->uuid_entity_map.begin(), offset);
				for(std::size_t j = 0; j < object_count; j++)
				{
					lua_str += std::format("rn.entity.update({}, {})", this->entities[(begin++)->second].ent.uuid, delta_seconds);
				}
				// execute update on all entities
				tz::lua::get_state().execute(lua_str.c_str());
				// those entity updates probably generated a bunch of messages.
				// as the global message receiver is completely thread-safe, we can concurrently send them over with other threads.
				// there will be some lock contention, but very minor - as there is a single lock per worker, no matter how many messages it generates.
				game::messaging::scene_messaging_local_dispatch();
			});
		}
		// do remainder objects ourselves.
		if(remainder_objects > 0)
		{
			TZ_PROFZONE("scene - update remaining objects on main thread", 0xFFCCAACC);
			std::string lua_str;
			for(std::size_t i = (count - remainder_objects); i < count; i++)
			{
				auto begin = std::next(this->uuid_entity_map.begin(), i);
				lua_str += std::format("rn.entity.update({}, {})", this->entities[(begin++)->second].ent.uuid, delta_seconds);
			}
			tz::lua::get_state().execute(lua_str.c_str());
			game::messaging::scene_messaging_local_dispatch();
		}
	}

	void scene::fixed_update(float delta_seconds, std::uint64_t unprocessed)
	{
		TZ_PROFZONE("scene - fixed update", 0xFFCCAACC);

		bool messages_sent = false;
		for(const auto& [entity_a, entity_b, manifold] : this->grid.get_intersections())
		{
			messages_sent |= this->single_collision_response(entity_a, entity_b, manifold);
		}
		if(messages_sent)
		{
			game::messaging::scene_messaging_local_dispatch();
			game::messaging::scene_messaging_update();
		}
	}

	void scene::block()
	{
		for(tz::job_handle jh : this->entity_update_jobs)
		{
			tz::job_system().block(jh);
		}
		this->entity_update_jobs.clear();
		this->renderer.block();
	}

	bool scene::contains_entity(entity_uuid uuid) const
	{
		return this->uuid_entity_map.contains(uuid);
	}

	bool scene::contains_entity(entity_handle e) const
	{
		return this->entities.contains(e);
	}

	std::size_t scene::entity_count() const
	{
		return this->entities.size();
	}

	const game::entity& scene::get_entity(entity_handle e) const
	{
		return this->entities[e].ent;
	}

	game::entity& scene::get_entity(entity_handle e)
	{
		return this->entities[e].ent;
	}

	const game::entity& scene::get_entity(entity_uuid uuid) const
	{
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		return this->get_entity(iter->second);
	}

	game::entity& scene::get_entity(entity_uuid uuid)
	{
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		return this->get_entity(iter->second);
	}

	const game::render::scene_renderer::entry& scene::get_entity_render_component(entity_uuid uuid) const
	{
		auto iter = this->uuid_entity_map.find(uuid);
		tz::assert(iter != this->uuid_entity_map.end(), "No entity exists with uuid %llu", uuid);
		return this->get_entity_render_component(iter->second);
	}

	const game::render::scene_renderer::entry& scene::get_entity_render_component(entity_handle e) const
	{
		return this->entities[e].ren;
	}

	const game::render::scene_renderer& scene::get_renderer() const
	{
		return this->renderer;
	}

	game::render::scene_renderer& scene::get_renderer()
	{
		return this->renderer;
	}

	physics::grid_hierarchy& scene::get_grid()
	{
		return this->grid;
	}

	const physics::grid_hierarchy& scene::get_grid() const
	{
		return this->grid;
	}

	void scene::notify_new_entity(entity_uuid uuid)
	{
		if(this->wants_collision_detection(uuid))
		{
			this->grid.add_entity(uuid, this->bound_entity(uuid), physics::grid_hierarchy::oob_policy::discard);
		}
	}

	void scene::notify_entity_change(entity_uuid uuid)
	{
		if(this->wants_collision_detection(uuid))
		{
			this->grid.notify_change(uuid, this->bound_entity(uuid), physics::grid_hierarchy::oob_policy::discard);
		}
	}

	tz::vec2 scene::get_mouse_position_world_space() const
	{
		return this->mouse_pos_ws;
	}

	physics::intersection_data_view scene::get_intersections()
	{
		return this->grid.get_intersections();
	}

	bool scene::wants_collision_detection(entity_uuid uuid) const
	{
		TZ_PROFZONE("scene - wants collision detection", 0xFFCC22CC);
		const auto& cmp = this->get_entity_render_component(uuid);
		if(cmp.model_name.empty())
		{
			// if it doesnt have a model name, that means it doesn't have a model.
			// if it doesnt have a model, then it has no transform (pos/rot/scale)
			// if it has no transform, the concept of collision detection makes no sense.
			return false;
		}
		// by this point it has a model and transform, but doesn't necessarily mean it cares about collisions.
		// if it doesnt have a prefab, then we assume it doesnt care about collisions.
		const auto& ent = this->get_entity(uuid);
		tz::lua::lua_generic maybe_prefab_name = ent.get_internal(".prefab");
		if(!std::holds_alternative<tz::lua::nil>(maybe_prefab_name))
		{
			// its not nil.
			// assert that its a string, as .prefab must be a string.
			tz::assert(std::holds_alternative<std::string>(maybe_prefab_name));
			auto prefab_name = std::get<std::string>(maybe_prefab_name);
			// try to get the prefab info from metadata.
			auto all_prefabs = game::meta::get_prefabs();
			auto iter = std::find_if(all_prefabs.begin(), all_prefabs.end(),
			[&prefab_name](const auto& prefabdata)
			{
				return prefabdata.name == prefab_name;
			});
			tz::assert(iter != all_prefabs.end());
			// if the prefab defines an `on_collision` method, then it does want collision detection.
			// if it doesn't then it doesnt need collision detection.
			return iter->has_on_collision;
		}
		return false;
	}

	physics::boundary_t scene::bound_entity(entity_uuid uuid) const
	{
		tz::lua::lua_generic maybe_custom_scale = this->get_entity(uuid).get_internal(".boundary_scale");
		float custom_scale = 1.0f;
		if(std::holds_alternative<double>(maybe_custom_scale))
		{
			custom_scale = std::get<double>(maybe_custom_scale);
		}
		auto comp = this->get_entity_render_component(uuid);
		if(!comp.model_name.empty())
		{
			tz::trs trs = this->get_renderer().get_renderer().animated_object_get_global_transform(comp.obj);
			tz::vec2 centre = trs.translate.swizzle<0, 1>();
			tz::vec2 extent = trs.scale.swizzle<0, 1>() * custom_scale;
			return {.min = centre - extent, .max = centre + extent};
		}
		else
		{
			tz::error("Cannot bound entity without render component");
			return {};
		}
	}

	void scene::initialise_renderer_component(entity_uuid uuid)
	{
		auto& ent = this->entities[this->uuid_entity_map.at(uuid)];
		tz::assert(ent.ren.obj == tz::nullhand, "initialise_render_component(ent) - already has an animated_objects handle!");
		ent.ren = this->renderer.add_entry(ent.ren.model_name);
		auto objs = this->renderer.get_renderer().animated_object_get_subobjects(ent.ren.obj);
		// note: entity lua code can set textures/colours on their renderer component elements.
		// however, this can never happen on the frame that the entity is created
		// for that reason, we instantly set all created objects to be invisible
		// otherwise, there may be a single-frame flash of an empty (default) texture before the necessary messages actually get processed.
		for(tz::ren::animation_renderer::object_handle oh : objs)
		{
			this->renderer.get_renderer().object_set_visible(oh, false);
		}
	}

	tz::vec2 scene::calc_mouse_position_world_space() const
	{
		auto windims = tz::window().get_dimensions();	
		auto mondims = tz::wsi::get_monitors().front().dimensions;
		const float ar = static_cast<float>(mondims[0]) / mondims[1];
		auto pos = static_cast<tz::vec2>(tz::window().get_mouse_state().mouse_position);
		// invert y
		pos[1] = windims[1] - pos[1];
		// transform to 0.0-1.0
		pos[0] /= windims[0];
		pos[1] /= windims[1];
		pos *= 2.0f;
		pos -= tz::vec2::filled(1.0f);
		// multiply by view bounds
		const tz::vec2 vb = this->get_renderer().get_view_bounds();
		pos[0] *= vb[0];
		pos[1] *= vb[1] / ar;
		// now translate by camera position
		const tz::vec2 campos = this->get_renderer().get_camera_position();
		pos += campos;
		return pos;
	}

	bool scene::single_collision_response(entity_uuid entity_a, entity_uuid entity_b, physics::boundary_t::manifold manifold)
	{
		TZ_PROFZONE("scene - collision response", 0xFFCC22CC);
		{
			TZ_PROFZONE("lua collision check", 0xFFCC22CC);
			std::string do_collision_check_lua = std::format("impl_do_collision_response = rn.entity.on_collision({}, {})", entity_a, entity_b);
			tz::lua::get_state().execute(do_collision_check_lua.c_str());
		}
		if(tz::lua::get_state().get_bool("impl_do_collision_response") == false)
		{
			return false;
		}
		auto comp_a = this->get_entity_render_component(entity_a);
		tz::assert(!comp_a.model_name.empty());
		auto comp_b = this->get_entity_render_component(entity_b);
		tz::assert(!comp_b.model_name.empty());

		tz::vec3 apos = this->get_renderer().get_renderer().animated_object_get_global_transform(comp_a.obj).translate;
		tz::vec3 bpos = this->get_renderer().get_renderer().animated_object_get_global_transform(comp_b.obj).translate;

		// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf
		// projection method - change their position in opposite directions along the manifold normal.
		// todo: consider impulse method
		tz::vec2 normal = manifold.normal.normalised() * manifold.penetration_depth;
		// move both entities.
		// note: we have a normal, but we dont know which should go along the normal and which should go opposite it.
		// to determine this, we dot product the displacement between the two with the normal.
		tz::vec2 displacement = bpos.swizzle<0, 1>() - apos.swizzle<0, 1>();
		float dot = displacement.dot(normal);
		if(dot > 0.0f)
		{
			normal *= -1.0f;
		}

		// get the mass ratio
		// if an entity has no `mass` internal variable, we assume its mass is 1.0.
		tz::lua::lua_generic mass_a_gen = this->get_entity(entity_a).get_internal("mass");
		tz::lua::lua_generic mass_b_gen = this->get_entity(entity_b).get_internal("mass");
		float mass_a = 1.0f;
		float mass_b = 1.0f;
		if(std::holds_alternative<double>(mass_a_gen))
		{
			mass_a = std::get<double>(mass_a_gen);
		}
		if(std::holds_alternative<std::int64_t>(mass_a_gen))
		{
			mass_a = std::get<std::int64_t>(mass_a_gen);
		}
		if(std::holds_alternative<double>(mass_b_gen))
		{
			mass_b = std::get<double>(mass_b_gen);
		}
		if(std::holds_alternative<std::int64_t>(mass_b_gen))
		{
			mass_b = std::get<std::int64_t>(mass_b_gen);
		}
		tz::assert(mass_a > 0.0f && mass_b > 0.0f);
		float mass_ratio = mass_a / (mass_a + mass_b);

		tz::vec2 disp_a = normal * (1.0f - mass_ratio);
		tz::vec2 disp_b = normal * -1.0f * (mass_ratio);

		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::entity_set_global_position,
			.uuid = entity_a,
			.value = (apos.swizzle<0, 1>() + disp_a).with_more(apos[2])
		});
		game::messaging::scene_insert_message
		({
			.operation = game::messaging::scene_operation::entity_set_global_position,
			.uuid = entity_b,
			.value = (bpos.swizzle<0, 1>() + disp_b).with_more(bpos[2])
		});
		return true;
	}

	decltype(scene::entities)::iterator scene::begin()
	{
		return this->entities.begin();
	}

	decltype(scene::entities)::const_iterator scene::begin() const
	{
		return this->entities.begin();
	}

	decltype(scene::entities)::iterator scene::end()
	{
		return this->entities.end();
	}

	decltype(scene::entities)::const_iterator scene::end() const
	{
		return this->entities.end();
	}
}
