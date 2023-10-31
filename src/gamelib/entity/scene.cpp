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
		tz::trs transform = ren.animated_object_get_local_transform(elem.entry.obj);
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

	std::size_t scene::get_collision_count(entity_handle e) const
	{
		return this->get_collision_count(this->get(e).uid);
	}

	std::size_t scene::get_collision_count(std::size_t uid) const
	{
		return this->collision_data[uid].size();
	}

	scene::entity_handle scene::get_collision_id(entity_handle e, std::size_t collision_id) const
	{
		return this->get_collision_id(this->get(e).uid, collision_id);
	}

	scene::entity_handle scene::get_collision_id(std::size_t uid, std::size_t collision_id) const
	{
		auto& container = this->collision_data[uid];
		auto iter = container.begin();
		tz::assert(collision_id < container.size());
		std::advance(iter, collision_id);
		return *iter;
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
		std::string cmd = "rn.entity.data[" + std::to_string(this->entities[hanval].uid) + "] = nil";
		tz::lua::get_state().execute(cmd.c_str());
		this->deinitialise_entity(static_cast<tz::hanval>(hanval), this->entities[hanval].uid);
		this->entities[hanval] = {.type = std::numeric_limits<std::size_t>::max()};
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
		for(std::size_t i = 0; i < this->size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			if(this->is_valid(hanval))
			{
				this->entities[i].update(delta_seconds);
			}
		}
		this->rebuild_quadtree();
	}

	void scene::dbgui()
	{
		this->dbgui_impl();
	}

	void scene::dbgui_game_bar()
	{
		ImGui::Text("Scene size: %zu", this->size());
		ImGui::SameLine();
		ImGui::Text("| %zu intersections | ", this->debug_get_intersection_count());
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[](const entity& ent)
		{
			// player melistra
			return ent.type == 0;
		});

		if(iter != this->entities.end())
		{
			ImGui::SameLine();
			ImGui::Text("%s %llu/%llu hp (%.1f%%)", iter->name.c_str(), iter->current_health, iter->get_stats().maximum_health, 100.0f * iter->current_health / iter->get_stats().maximum_health);
		}
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
		// note: this doesnt work. something to do with re-using gltfs im pretty sure.
		// todo: fix this engine-side, as draw count never stops increasing even when we recycle stuff :(
		this->get_renderer().remove_model(ent.elem.entry);
	}

	void scene::rebuild_quadtree()
	{
		this->quadtree.clear();
		this->collision_data.clear();
		for(std::size_t i = 0; i < this->size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			if(this->is_valid(hanval))
			{
				this->quadtree.add({.sc = this, .entity_hanval = hanval});
			}
		}
		this->intersection_state = this->quadtree.find_all_intersections();
		for(const auto [node_a, node_b] : intersection_state)
		{
			auto uid_a = this->get(node_a.entity_hanval).uid;
			auto uid_b = this->get(node_b.entity_hanval).uid;
			this->collision_data[uid_a].insert(node_b.entity_hanval);
			this->collision_data[uid_b].insert(node_a.entity_hanval);
		}
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

	int rn_impl_scene::get_collision_count(tz::lua::state& state)
	{
		auto [_, uid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		state.stack_push_uint(this->sc->get_collision_count(static_cast<std::size_t>(static_cast<tz::hanval>(uid))));
		return 1;
	}

	int rn_impl_scene::get_collision(tz::lua::state& state)
	{
		auto [_, uid, id] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
		auto colliding_eh = this->sc->get_collision_id(static_cast<std::size_t>(static_cast<tz::hanval>(uid)), id);
		state.stack_push_uint(static_cast<std::size_t>(static_cast<tz::hanval>(colliding_eh)));
		return 1;
	}

	int rn_impl_scene::get(tz::lua::state& state)
	{
		auto [_, eh] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		rn_impl_entity ent{.scene = this->sc, .entity_hanval = static_cast<tz::hanval>(eh)};
		LUA_CLASS_PUSH(state, rn_impl_entity, ent);
		return 1;
	}

	int rn_impl_scene::get_uid(tz::lua::state& state)
	{
		auto [_, uid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
		rn_impl_entity ent{};
		for(std::size_t i = 0; i < this->sc->size(); i++)
		{
			auto hv = static_cast<tz::hanval>(i);
			if(this->sc->get(hv).uid == uid)
			{
				ent = {.scene = this->sc, .entity_hanval = hv};
				break;
			}
		}
		if(ent.scene == nullptr)
		{
			state.stack_push_nil();
		}
		else
		{
			LUA_CLASS_PUSH(state, rn_impl_entity, ent);
		}
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

	int rn_impl_scene::get_mouse_position_ws(tz::lua::state& state)
	{
		auto windims = tz::window().get_dimensions();
		auto pos = static_cast<tz::vec2>(tz::window().get_mouse_state().mouse_position);
		// invert y
		pos[1] = windims[1] - pos[1];
		// transform to 0.0-1.0
		pos[0] /= windims[0];
		pos[1] /= windims[1];
		pos *= 2.0f;
		pos -= tz::vec2::filled(1.0f);
		// multiply by view bounds
		const tz::vec2 vb = this->sc->get_renderer().get_view_bounds();
		pos[0] *= vb[0];
		pos[1] *= vb[1];
		// now translate by camera position
		const tz::vec2 campos = this->sc->get_renderer().get_camera_position();
		pos += campos;

		state.stack_push_int(pos[0]);
		state.stack_push_int(pos[1]);
		return 2;
	}

	// dbgui

	void scene::dbgui_impl()
	{
		ImGui::TextColored(ImVec4{1.0f, 0.3f, 0.3f, 1.0f}, "ENTITIES LIST");
		ImGui::Spacing();
		
		std::size_t resident_count = std::count_if(this->entities.begin(), this->entities.end(),
		[](const entity& ent)
		{
			return ent.type != std::numeric_limits<std::size_t>::max();
		});

		ImGui::Text("%zu entities (%zu resident, %zu free-list)", this->size(), resident_count, this->size() - resident_count);
		constexpr float slider_height = 160.0f;
		static int entity_id = 0;
		if(ImGui::Button("+"))
		{
			entity_id = std::min(entity_id + 1, static_cast<int>(this->size()) - 1);
		}
		ImGui::VSliderInt("##entityid", ImVec2{18.0f, slider_height}, &entity_id, 0, this->entities.size() - 1);
		auto& ent = this->entities[entity_id];
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
		if(ImGui::BeginChild("#who_asked", ImVec2(0, slider_height), false, ImGuiWindowFlags_ChildWindow))
		{
			if(ent.type == std::numeric_limits<std::size_t>::max())
			{
				// its a deleted entity
				ImGui::Text("Deleted Entity");
				if(entity_id > 0)
				{
					if(ImGui::Button("\\/\\/"))
					{
						while(entity_id > 0 && this->entities[entity_id].type == std::numeric_limits<std::size_t>::max())
						{
							entity_id--;
						}
					}
					ImGui::SameLine();
					if(ImGui::Button("\\/"))
					{
						entity_id--;
					}
				}
				ImGui::SameLine();
				if(entity_id < (this->size() - 1))
				{
					if(ImGui::Button("^"))
					{
						entity_id++;	
					}
					ImGui::SameLine();
					if(ImGui::Button("^^"))
					{
						while(entity_id < (this->size() - 1) && this->entities[entity_id].type == std::numeric_limits<std::size_t>::max())
						{
							entity_id++;
						}
					}
				}
			}
			else
			{
				ent.dbgui();
				ImGui::Separator();
				if(ImGui::Button("Remove"))
				{
					this->remove(static_cast<tz::hanval>(entity_id));
				}
			}
		}
		ImGui::EndChild();
		if(ImGui::Button("-"))
		{
			if(entity_id > 0)
			{
				entity_id--;
			}
		}
	}
}