#include "gamelib/entity/scene.hpp"
#include "gamelib/entity/api.hpp"
#include "tz/core/imported_text.hpp"
#include "tz/lua/state.hpp"
#include "tz/wsi/monitor.hpp"

#include ImportedTextHeader(entity, lua)

namespace game::entity
{
	game::physics::aabb scene_quadtree_node::get_aabb() const
	{
		entity& e = this->sc->get(this->entity_hanval);
		auto& elem = e.elem;
		// get first object, represent its scale and position as a very basic aabb

		auto& ren = elem.renderer->get_renderer();
		tz::trs transform = ren.animated_object_get_global_transform(elem.entry.obj);
		tz::vec2 position = transform.translate.swizzle<0, 1>();
		tz::vec2 half_scale = transform.scale.swizzle<0, 1>();// * 0.5f; // half seems a bit off. keep it as-is.
		if(elem.get_model() == game::render::scene_renderer::model::humanoid)
		{
			// humanoid needs a little correction.
			position[1] += half_scale[1];
			half_scale *= tz::vec2{8.0f, 5.0f};
		}
		else if(elem.get_model() == game::render::scene_renderer::model::quad)
		{
			half_scale *= {0.5f, 0.5f};
		}
		// min is position - half_scale
		// max is position + half_scale
		return {position - half_scale, position + half_scale};
	}

	const render::scene_renderer& scene::get_renderer() const
	{
		return this->renderer;
	}

	render::scene_renderer& scene::get_renderer()
	{
		return this->renderer;
	}

	unsigned int scene::get_player_credits() const
	{
		return tz::lua::get_state().get_uint("rn.player_credits").value_or(0u);
	}

	void scene::set_player_credits(unsigned int credits)
	{
		tz::lua::get_state().assign_uint("rn.player_credits", credits);
	}

	tz::vec2 scene::get_mouse_position_ws() const
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
		this->deinitialise_entity(static_cast<tz::hanval>(hanval), this->entities[hanval].uid);
		this->entities[hanval] = {.type = std::numeric_limits<std::size_t>::max()};
	}

	void scene::clear()
	{
		for(std::size_t eid = 0; eid < this->entities.size(); eid++)
		{
			auto hv = static_cast<tz::hanval>(eid);
			if(this->is_valid(hv))
			{
				this->deinitialise_entity(hv, this->entities[eid].uid);
			}
		}
		this->entities.clear();
		this->free_list.clear();
		this->light_free_list.clear();
		this->light_cursor = 0;
		this->quadtree.clear();
		this->intersection_state = {};
		this->collision_data = {};
	}

	void scene::clear_except_players()
	{
		for(std::size_t i = 0; i < this->entities.size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			if(this->is_valid(hanval) && this->entities[i].type != 0)
			{
				this->remove(hanval);
			}
		}
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

	scene::light_handle scene::add_light(render::scene_renderer::point_light_data d)
	{
		std::size_t hanval = this->light_cursor;
		if(this->light_free_list.size())
		{
			hanval = static_cast<std::size_t>(static_cast<tz::hanval>(this->light_free_list.front()));
			this->light_free_list.pop_front();
		}
		else
		{
			this->light_cursor++;
		}
		if(hanval >= this->renderer.get_point_lights().size())
		{
			// ran out of lights. uhhh
			// re-use the last one (bad)
			// double the capacity.
			std::size_t old_cap = this->renderer.get_point_lights().size();
			std::size_t new_cap = old_cap * 2;
			this->renderer.set_point_light_capacity(new_cap);
		}
		tz::assert(hanval < this->renderer.get_point_lights().size(), "Ran outta lights innit");
		this->renderer.get_point_lights()[hanval] = d;
		return static_cast<tz::hanval>(hanval);
	}

	void scene::remove_light(light_handle l)
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(l));
		tz::assert(std::find(this->light_free_list.begin(), this->light_free_list.end(), l) == this->light_free_list.end(), "Double-free on light handle %zu", l);
		this->light_free_list.push_back(l);
		this->renderer.get_point_lights()[hanval] = {};
	}
	
	const render::scene_renderer::point_light_data& scene::get_light(light_handle l) const
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(l));
		return this->renderer.get_point_lights()[hanval];
	}

	render::scene_renderer::point_light_data& scene::get_light(light_handle l) 
	{
		auto hanval = static_cast<std::size_t>(static_cast<tz::hanval>(l));
		return this->renderer.get_point_lights()[hanval];
	}

	void scene::update(float delta_seconds)
	{
		for(std::size_t i = 0; i < this->size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			if(this->is_valid(hanval))
			{
				this->entities[i].update(delta_seconds);
			}
		}
		this->rebuild_quadtree();
		this->collision_response(delta_seconds);
		this->advance_camera(delta_seconds);
		this->renderer.update(delta_seconds);
	}

	void scene::block()
	{
		this->renderer.block();
	}

	void scene::dbgui()
	{
		this->dbgui_impl();
	}

	void scene::dbgui_game_bar()
	{
		ImGui::Text("Scene size: %zu", this->size());
		ImGui::SameLine();
		tz::vec2 pos = this->get_mouse_position_ws();
		ImGui::Text("| mouse %.1f, %.1f | ", pos[0], pos[1]);
		ImGui::SameLine();
		ImGui::Text("| %zu intersections | ", this->debug_get_intersection_count());
		entity_handle player = this->try_find_player();

		if(player != tz::nullhand)
		{
			auto& p = this->get(player);
			ImGui::SameLine();
			ImGui::Text("%s %llu/%llu hp (%.1f%%)", p.name.c_str(), p.current_health, p.get_stats().maximum_health, 100.0f * p.current_health / p.get_stats().maximum_health);
			auto stats = p.get_stats();
			ImGui::SameLine();
			ImGui::Text("Speed: %.2f", stats.movement_speed);
			ImGui::SameLine();
			ImGui::Text("Attack Power: %llu", stats.attack_power);
			ImGui::SameLine();
			ImGui::Text("Spell Power: %llu", stats.spell_power);
			ImGui::SameLine();
			ImGui::Text("Defence: %llu", stats.defence_rating);
		}

		ImGui::SameLine();
		ImGui::Text("| Credits: %u", this->get_player_credits());
	}

	void scene::lua_initialise(tz::lua::state& state)
	{
		TZ_PROFZONE("scene - lua initialise", 0xFF99CC44);
		this->get_renderer().lua_initialise(state);
		state.new_type("rn_impl_entity", LUA_CLASS_NAME(rn_impl_entity)::registers);
		state.new_type("rn_impl_scene", LUA_CLASS_NAME(rn_impl_scene)::registers);
		state.new_type("rn_impl_light", LUA_CLASS_NAME(rn_impl_light)::registers);
		
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

	scene::entity_handle scene::try_find_player() const
	{
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[](const entity& ent)
		{
			// player melistra
			return ent.type == 0;
		});

		if(iter == this->entities.end())
		{
			return tz::nullhand;
		}
		return static_cast<tz::hanval>(std::distance(this->entities.begin(), iter));
	}

	void scene::rebuild_quadtree()
	{
		TZ_PROFZONE("scene - rebuild quadtree", 0xFF99CC44);
		this->quadtree.clear();
		this->collision_data.clear();
		for(std::size_t i = 0; i < this->size(); i++)
		{
			auto hanval = static_cast<tz::hanval>(i);
			const auto& entity = this->get(hanval);
			if(this->is_valid(hanval) && !entity.flags.contains(flag::no_collide))
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

	void scene::collision_response(float delta_seconds)
	{
		TZ_PROFZONE("scene - collision response", 0xFF99CC44);
		for(const auto& [node_a, node_b] : this->intersection_state)
		{
			tz::hanval enta = node_a.entity_hanval;
			tz::hanval entb = node_b.entity_hanval;
			this->resolve_collision(enta, entb, delta_seconds);
			this->resolve_collision(entb, enta, delta_seconds);
		}
	}

	void scene::resolve_collision(entity_handle ah, entity_handle bh, float delta_seconds)
	{
		TZ_PROFZONE("scene - resolve collision", 0xFF99CC44);
		entity& a = this->get(ah);
		entity& b = this->get(bh);
		if(a.current_health == 0 || b.current_health == 0)
		{
			return;
		}
		auto calculate_overlap = [](float min1, float max1, float min2, float max2)
		{
			if(max1 <= min2 || max2 <= min1)
			{
				return 0.0f;
			}
			else
			{
				return std::min(max1, max2) - std::max(min1, min2);
			}
		};
		game::physics::aabb a_box = scene_quadtree_node{.sc = this, .entity_hanval = static_cast<tz::hanval>(ah)}.get_aabb();
		game::physics::aabb b_box = scene_quadtree_node{.sc = this, .entity_hanval = static_cast<tz::hanval>(bh)}.get_aabb();
		float overlap_x = calculate_overlap(b_box.get_left(), b_box.get_right(), a_box.get_left(), a_box.get_right());
		float overlap_y = calculate_overlap(b_box.get_bottom(), b_box.get_top(), a_box.get_bottom(), a_box.get_top());
		float correction = std::min(overlap_x, overlap_y) * 0.5f;
		if(b.flags.contains(flag::immoveable_collide))
		{
			return;
		}
		if(overlap_x < overlap_y)
		{
			if(b_box.get_centre()[0] > a_box.get_centre()[0])
			{
				// move bx by +correction
				tz::trs trs = this->renderer.get_renderer().animated_object_get_local_transform(b.elem.entry.obj);
				trs.translate[0] += correction;
				this->renderer.get_renderer().animated_object_set_local_transform(b.elem.entry.obj, trs);
			}
			else
			{
				// move bx by -correction
				tz::trs trs = this->renderer.get_renderer().animated_object_get_local_transform(b.elem.entry.obj);
				trs.translate[0] -= correction;
				this->renderer.get_renderer().animated_object_set_local_transform(b.elem.entry.obj, trs);
			}
		}
		else
		{
			if(b_box.get_centre()[1] > a_box.get_centre()[1])
			{
				// move by by +correction
				tz::trs trs = this->renderer.get_renderer().animated_object_get_local_transform(b.elem.entry.obj);
				trs.translate[1] += correction;
				this->renderer.get_renderer().animated_object_set_local_transform(b.elem.entry.obj, trs);
			}
			else
			{
				// move by by -correction
				tz::trs trs = this->renderer.get_renderer().animated_object_get_local_transform(b.elem.entry.obj);
				trs.translate[1] -= correction;
				this->renderer.get_renderer().animated_object_set_local_transform(b.elem.entry.obj, trs);
			}
		}
	}

	void scene::advance_camera(float delta_seconds)
	{
		entity_handle player = this->try_find_player();
		if(player == tz::nullhand)
		{
			return;
		}
		auto& p = this->get(player);

		tz::trs player_trs = this->renderer.get_renderer().animated_object_get_local_transform(p.elem.entry.obj);
		tz::vec2 player_pos = player_trs.translate.swizzle<0, 1>();
		tz::vec2 diff = player_pos - this->renderer.get_camera_position();
		constexpr float cam_dist_move_diff = 8.0f;
		if(diff.length() >= cam_dist_move_diff)
		{
			tz::vec2 new_cam_pos = this->renderer.get_camera_position() + (diff * 0.01f);
			this->renderer.set_camera_position(new_cam_pos);
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

	int rn_impl_scene::add_light(tz::lua::state& state)
	{
		scene::light_handle l = this->sc->add_light
		({
		});
		rn_impl_light light{.sc = this->sc, .l = l};
		LUA_CLASS_PUSH(state, rn_impl_light, light);
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

	int rn_impl_scene::remove_light(tz::lua::state& state)
	{
		auto& light = state.stack_get_userdata<rn_impl_light>(2);
		this->sc->remove_light(light.l);
		return 0;
	}

	int rn_impl_scene::clear(tz::lua::state& state)
	{
		this->sc->clear();
		return 0;
	}

	int rn_impl_scene::clear_except_players(tz::lua::state& state)
	{
		this->sc->clear_except_players();
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
		tz::vec2 pos = this->sc->get_mouse_position_ws();	
		state.stack_push_float(pos[0]);
		state.stack_push_float(pos[1]);
		return 2;
	}

	int rn_impl_light::get_position(tz::lua::state& state)
	{
		const auto& light = this->sc->get_light(this->l);
		state.stack_push_float(light.position[0]);
		state.stack_push_float(light.position[1]);
		return 2;
	}

	int rn_impl_light::set_position(tz::lua::state& state)
	{
		auto [_, x, y] = tz::lua::parse_args<tz::lua::nil, float, float>(state);
		auto& light = this->sc->get_light(this->l);
		light.position[0] = x;
		light.position[1] = y;
		return 0;
	}

	int rn_impl_light::get_colour(tz::lua::state& state)
	{
		const auto& light = this->sc->get_light(this->l);
		state.stack_push_float(light.colour[0]);
		state.stack_push_float(light.colour[1]);
		state.stack_push_float(light.colour[2]);
		return 3;
	}

	int rn_impl_light::set_colour(tz::lua::state& state)
	{
		auto [_, r, g, b] = tz::lua::parse_args<tz::lua::nil, float, float, float>(state);
		auto& light = this->sc->get_light(this->l);
		light.colour[0] = r;
		light.colour[1] = g;
		light.colour[2] = b;
		return 0;
	}

	int rn_impl_light::get_power(tz::lua::state& state)
	{
		const auto& light = this->sc->get_light(this->l);
		state.stack_push_float(light.power);
		return 1;
	}

	int rn_impl_light::set_power(tz::lua::state& state)
	{
		auto [_, power] = tz::lua::parse_args<tz::lua::nil, float>(state);
		auto& light = this->sc->get_light(this->l);
		light.power = power;
		return 0;
	}

	// dbgui

	void scene::dbgui_impl()
	{
		ImGui::TextColored(ImVec4{1.0f, 0.3f, 0.3f, 1.0f}, "ENTITIES LIST");
		ImGui::Spacing();
		if(ImGui::Button("Clear"))
		{
			this->clear();
			return;
		}
		
		std::size_t resident_count = std::count_if(this->entities.begin(), this->entities.end(),
		[](const entity& ent)
		{
			return ent.type != std::numeric_limits<std::size_t>::max();
		});

		ImGui::Text("%zu entities (%zu resident, %zu free-list)", this->size(), resident_count, this->size() - resident_count);
		constexpr float slider_height = 160.0f;
		static int entity_id = 0;
		if(this->entities.empty())
		{
			return;
		}
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

		tz::vec2 cam_pos = this->renderer.get_camera_position();
		if(ImGui::SliderFloat2("Camera Position", cam_pos.data().data(), -2.0f, 2.0f))
		{
			this->renderer.set_camera_position(cam_pos);
		}
	}
}