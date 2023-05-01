#include "gamelib/gameplay/actor/system.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "tz/core/job/job.hpp"
#include "tz/core/algorithms/static.hpp"

namespace rnlib
{
	actor& actor_system::add(actor_type t)
	{
		this->entities.push_back(rnlib::create_actor(t));
		return this->entities.back();
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

	void actor_system::set_intersection_state(actor_quadtree::intersection_state_t state)
	{
		this->intersection_state = state;
	}

	void actor_system::update(float dt, update_context ctx)
	{
		TZ_PROFZONE("actor_system - update", 0xffee0077);
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

		// remove all entities that need to be deleted.
		this->entities.erase(std::remove_if(this->entities.begin(), this->entities.end(),
		[this](const actor& a)
		{
			return std::find(this->entities_to_delete.begin(), this->entities_to_delete.end(), a.uuid) != this->entities_to_delete.end();
		}), this->entities.end());
		this->entities_to_delete.clear();
	}

	void actor_system::dbgui()
	{
		if(ImGui::BeginTabBar("Actor"))
		{
			if(ImGui::BeginTabItem("Actors List"))
			{
				static int entity_id = 0;
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
					tz::assert(lactor != nullptr);
					tz::assert(ractor != nullptr);
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
					}
					if(ImGui::Button("Clear"))
					{
						this->entities.clear();
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
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
}
