#include "gamelib/gameplay/actor/system.hpp"
#include "gamelib/gameplay/actor/combat.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/job/job.hpp"
#include "tz/core/algorithms/static.hpp"

namespace rnlib
{
	actor& actor_system::add(actor_type t)
	{
		this->entities_to_add.push_back(rnlib::create_actor(t));
		return this->entities_to_add.back();
	}

	const actor* actor_system::find(std::size_t uuid) const
	{
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[uuid](const actor& entity)
		{
			return entity.uuid == uuid;
		});
		if(iter == this->entities.end())
		{
			return nullptr;
		}
		return &*iter;
	}

	actor* actor_system::find(std::size_t uuid)
	{
		auto iter = std::find_if(this->entities.begin(), this->entities.end(),
		[uuid](const actor& entity)
		{
			return entity.uuid == uuid;
		});
		if(iter == this->entities.end())
		{
			return nullptr;
		}
		return &*iter;
	}

	void actor_system::remove(std::size_t uuid)
	{
		this->entities_to_delete.push_back(uuid);
	}

	mount_result actor_system::mount(std::span<quad_renderer::quad_data> quads)
	{
		mount_result res;
		for(const actor& entity : this->entities)
		{
			res << entity.mount(quads.subspan(res.count));
		}
		return res;
	}

	std::size_t actor_system::size() const
	{
		return this->entities.size();
	}

	std::span<actor> actor_system::container()
	{
		return this->entities;
	}

	std::span<const actor> actor_system::container() const
	{
		return this->entities;
	}

	void actor_system::add_to_combat_log(combat_event evt)
	{
		this->combat_events_this_frame.push_back(evt);
	}

	void actor_system::add_to_combat_log(std::span<const combat_event> evts)
	{
		for(combat_event evt : evts)
		{
			this->add_to_combat_log(evt);
		}
	}

	void actor_system::set_intersection_state(actor_quadtree::intersection_state_t state)
	{
		this->intersection_state = state;
	}

	void actor_system::update(float dt, update_context ctx)
	{
		TZ_PROFZONE("actor_system - update", 0xffee0077);
		this->collision_response();
		constexpr std::size_t arbitrary_serial_update_max = 250;
		if(this->entities.size() < arbitrary_serial_update_max)
		{
			TZ_PROFZONE("actor_system - serial update", 0xffee0077);
			// if we dont have that many entities, do them all now.
			for(auto& entity : this->entities)
			{
				entity.update(dt);		
			}
		}
		else
		{
			// otherwise, do a multi-threaded approach.
			const std::size_t ecount = this->entities.size();
			const std::size_t job_count = std::thread::hardware_concurrency();
			std::size_t job_batch_size = ecount / job_count;
			std::size_t remainder_jobs = ecount % job_count;
			std::vector<tz::job_handle> jobs(job_count);
			for(std::size_t i = 0; i < job_count; i++)
			{
				jobs[i] = tz::job_system().execute([this, i, dt, job_batch_size](){this->update_n(i * job_batch_size, job_batch_size, dt);});
			}
			TZ_PROFZONE("actor_system - parallel update", 0xffee0077);
			this->update_n(ecount - remainder_jobs, remainder_jobs, dt);
			for(tz::job_handle jh : jobs)
			{
				tz::job_system().block(jh);
			}
		}

		// now go ahead and pump actions.
		{
			TZ_PROFZONE("actor_system - actions advance", 0xffee0077);
			for(auto& entity : this->entities)
			{
				#define HANDLE_ACTION(T) if(entity.actions.has_component<T>()){rnlib::action_invoke<T>(*this, entity, *entity.actions.get_component<T>());}
				tz::static_for<0, static_cast<int>(action_id::_count)>([this, &entity, &ctx](auto i)
				{
					constexpr auto id = static_cast<action_id>(static_cast<int>(i));
					if(entity.actions.has_component<id>())
					{
						rnlib::action_invoke<id>(*this, entity, *entity.actions.get_component<id>(), ctx);
					}
				});
				entity.actions.update();
			}
		}

		const auto& mouse = tz::window().get_mouse_state();
		// handle spell casts and click input.
		for(auto& entity : this->entities)
		{
			if(entity.entity.has_component<actor_component_id::level_background>())
			{
				auto [min, max] = ctx.level_bounds;
				entity.transform.local_position = (min + max) / 2.0f;
				entity.transform.local_scale = (max - min) * 0.5f;
			}
			if(entity.entity.has_component<actor_component_id::cast>())
			{
				TZ_PROFZONE("actor_system - handle spell cast", 0xffee0077);
				auto& cast = entity.entity.get_component<actor_component_id::cast>()->data();
				if(cast.complete())
				{
					// do the cast effect.
					auto events = cast.spell.function(entity, *this, ctx);
					for(auto& evt : events)
					{
						evt.spell = cast.spell.id;
					}
					this->add_to_combat_log(events);
					entity.entity.remove_component<actor_component_id::cast>();
				}
			}
			if(entity.entity.has_component<actor_component_id::action_listener>())
			{
				TZ_PROFZONE("actor_system - handle click action_listener", 0xffee0077);
				auto& action_listener = entity.entity.get_component<actor_component_id::action_listener>()->data();
				if(action_listener.on_click.size())
				{
					tz::static_for<0, static_cast<int>(tz::wsi::mouse_button::_count)>([this, &mouse, &action_listener, &entity](auto i)
					{
						auto button = static_cast<tz::wsi::mouse_button>(static_cast<int>(i));
						if(tz::wsi::is_mouse_button_down(mouse, button) && !tz::dbgui::claims_mouse())
						{
							TZ_PROFZONE("actor_system - action_listener trigger on click", 0xffee0077);
							for(const auto& func : action_listener.on_click)
							{
								func(entity, button);
							}
						}
					});
				}
			}
		}

		// remove all entities that need to be deleted.
		this->entities.erase(std::remove_if(this->entities.begin(), this->entities.end(),
		[this](const actor& a)
		{
			return std::find(this->entities_to_delete.begin(), this->entities_to_delete.end(), a.uuid) != this->entities_to_delete.end();
		}), this->entities.end());
		this->entities_to_delete.clear();
		for(actor a : this->entities_to_add)
		{
			this->entities.push_back(a);
		}
		this->entities_to_add.clear();

		this->send_combat_text();
	}

	void actor_system::dbgui()
	{
		static int entity_id = 0;
		static bool display_all_colliders = true;
		if(ImGui::BeginTabBar("Actor"))
		{
			if(ImGui::BeginTabItem("Actors List"))
			{
				if(this->entities.size())
				{
					ImGui::SliderInt("Entity ID", &entity_id, 0, this->entities.size() - 1);
					ImGui::Indent();
					this->entities[entity_id].dbgui();
					ImGui::Unindent();
				}
				else
				{
					ImGui::Text("No actors to display :(");
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Collision Detection"))
			{
				ImGui::Text("%zu intersections", this->intersection_state.size());
				for(const auto[lnode, rnode] : this->intersection_state)
				{
					actor* lactor = this->find(lnode.uuid);
					actor* ractor = this->find(rnode.uuid);
					if(lactor == nullptr || ractor == nullptr)
					{
						continue;
					}
					ImGui::Text("%zu (%s) <=> %zu (%s)", lactor->uuid, lactor->name, ractor->uuid, ractor->name);
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Debug Operations"))
			{
				static const char* combo_preview = "Actor types...";
				static std::optional<actor_type> maybe_type = std::nullopt;
				if(ImGui::BeginCombo("Create actor", combo_preview))
				{
					for(int i = 0 ; i < static_cast<int>(actor_type::_count); i++)
					{
						auto type = static_cast<actor_type>(i);
						if(type == actor_type::undefined)
						{
							continue;
						}
						bool is_selected = (maybe_type.has_value() && maybe_type.value() == type);
						const char* actor_name = create_actor(type).name;
						if(ImGui::Selectable(actor_name, is_selected))
						{
							maybe_type = type;
							combo_preview = actor_name;
						}
						if(is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if(maybe_type.has_value())
				{
					if(ImGui::Button("Create"))
					{
						this->entities.push_back(rnlib::create_actor(maybe_type.value()));
					}
				}
				if(this->entities.size())
				{
					if(ImGui::Button("Pop Back"))
					{
						this->remove(this->entities.back().uuid);
						if(entity_id >= (this->entities.size() - 1))
						{
							if(entity_id > 0)
							{
								entity_id--;
							}
						}
					}
					if(ImGui::Button("Clear"))
					{
						this->entities.clear();
						entity_id = 0;
					}
				}
				ImGui::Checkbox("Display All Colliders", &display_all_colliders);
				if(ImGui::Button("Add Level Background"))
				{
					auto& ent = this->entities.emplace_back();
					ent.entity.add_component<actor_component_id::level_background>
					({
						.background_texid = image_id::background_grassy,
		  				.background_texscale = 7.0f,
		  				.foreground_texid = image_id::foreground_blanchfield_cemetary
					});
					ent.layer = 100;
				}
				ImGui::EndTabItem();
			}
			if(ImGui::BeginTabItem("Combat Log"))
			{
				for(const combat_event& evt : combat::get_log().container())
				{
					spell sp = rnlib::create_spell(evt.spell);
					const actor* caster = this->find(evt.caster_uuid);
					const actor* target = this->find(evt.target_uuid);
					const char* caster_name = caster ? caster->name : "???";
					const char* target_name = target ? target->name : "???";
					const char* verb = "";
					const char* over_adj = "";
					switch(evt.type)
					{
						case combat_text_type::damage:
							verb = "hurt";
							over_adj = "kill";
						break;
						case combat_text_type::heal:
							verb = "healed";
							over_adj = "heal";
						break;
						case combat_text_type::immune:
							ImGui::Text("%s casted [%s] on %s, but was immune.", caster_name, sp.name, target_name);	
							continue;
						break;
						default:
							tz::error("unknown combat text type. please add support.");
						break;
					}
					std::string damage_type_name = "";
					const char* dtype = rnlib::combat::get_damage_type_name(evt.damage_type);
					if(std::strcmp(dtype, "") != 0)
					{
						damage_type_name = std::string(" ") + dtype;
					}
					ImGui::Text("%s's [%s] %s %s for %zu%s", caster_name, sp.name, verb, target_name, evt.value, damage_type_name.c_str());
					if(evt.over.has_value())
					{
						ImGui::SameLine();
						ImGui::Text("(over%s: %zu)", over_adj, evt.over.value());
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		for(auto& ent : this->entities)
		{
			if(ent.entity.has_component<actor_component_id::collide>())
			{
				auto& collide = ent.entity.get_component<actor_component_id::collide>()->data();
				collide.debug_draw = display_all_colliders;
				collide.debug_draw_colour = (ent.uuid == this->entities[entity_id].uuid) ? tz::vec3{1.0f, 0.0f, 0.0f} : tz::vec3::filled(1.0f);
			}
		}
	}

	void actor_system::update_one(std::size_t eid, float dt)
	{
		TZ_PROFZONE("actor_system - update one", 0xffee0077);
		this->entities[eid].update(dt);
	}

	void actor_system::update_n(std::size_t eid_begin, std::size_t n, float dt)
	{
		TZ_PROFZONE("actor_system - update n", 0xffee0077);
		for(std::size_t i = eid_begin; i < (eid_begin + n); i++)
		{
			tz::assert(i < this->entities.size());
			this->update_one(i, dt);
		}
	}

	void actor_system::collision_response()
	{
		TZ_PROFZONE("actor_system - collision resolution", 0xffee0077);
		for(const auto& [node_a, node_b] : this->intersection_state)
		{
			std::size_t uuid_a = node_a.uuid;
			std::size_t uuid_b = node_b.uuid;
			this->resolve_collision(uuid_a, uuid_b);
			this->resolve_collision(uuid_b, uuid_a);
		}
	}

	void actor_system::resolve_collision(std::size_t uuid_a, std::size_t uuid_b)
	{
		TZ_PROFZONE("actor_system - resolve collision", 0xffee0077);
		actor* a = this->find(uuid_a);
		actor* b = this->find(uuid_b);
		tz::assert(a != nullptr && b != nullptr);
		if(!a->entity.has_component<actor_component_id::collide>() || !b->entity.has_component<actor_component_id::collide>())
		{
			// unless both of them have a collide component, we ignore collisions.
			return;
		}
		auto& collide_a = a->entity.get_component<actor_component_id::collide>()->data();
		auto& collide_b = b->entity.get_component<actor_component_id::collide>()->data();
		if(!collide_a.collide_if(*a, *b) || !collide_b.collide_if(*b, *a) || b->dead() || a->dead())
		{
			// both actor colliders need to agree that they should collide. otherwise ignore.
			// also dead people can't collide.
			return;
		}
		// resolve collision by calculating bounding box overlap and resolving based on that displacement.
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
		box a_box = a->transform.get_bounding_box();
		box b_box = b->transform.get_bounding_box();
		float overlap_x = calculate_overlap(b_box.get_left(), b_box.get_right(), a_box.get_left(), a_box.get_right());
		float overlap_y = calculate_overlap(b_box.get_bottom(), b_box.get_top(), a_box.get_bottom(), a_box.get_top());
		float correction = std::min(overlap_x, overlap_y) * a->entity.get_component<actor_component_id::collide>()->data().mass_ratio;
		if(overlap_x < overlap_y)
		{
			// solve based on x.
			if(b_box.get_centre()[0] > a_box.get_centre()[0])
			{
				b->transform.local_position[0] += correction;
			}
			else
			{
				b->transform.local_position[0] -= correction;
			}
		}
		else
		{
			// solve based on y.
			if(b_box.get_centre()[1] > a_box.get_centre()[1])
			{
				b->transform.local_position[1] += correction;
			}
			else
			{
				b->transform.local_position[1] -= correction;
			}
		}

		// code on collision could happen here.
		if(a->entity.has_component<actor_component_id::action_listener>())
		{
			for(const auto& callable : a->entity.get_component<actor_component_id::action_listener>()->data().on_collide)
			{
				callable(*a, *b);
			}
		}
	}

	void actor_system::send_combat_text()
	{
		for(const combat_event& evt : this->combat_events_this_frame)
		{
			actor* a = this->find(evt.caster_uuid);
			actor* b = this->find(evt.target_uuid);
			if(a == nullptr || b == nullptr)
			{
				continue;
			}
			if(b->entity.has_component<actor_component_id::damageable>())
			{
				switch(evt.type)
				{
					case combat_text_type::damage:
						combat::damage(*b, a, evt.spell, evt.value, evt.damage_type);
					break;
					case combat_text_type::heal:
						combat::heal(*b, a, evt.spell, evt.value, evt.damage_type);
					break;
				}
			}
		}
		this->combat_events_this_frame.clear();
	}
}
