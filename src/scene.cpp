#include "scene.hpp"
#include "util.hpp"
#include "tz/dbgui/dbgui.hpp"
#include "hdk/job/job.hpp"
#include "hdk/debug.hpp"
#include "hdk/profile.hpp"

#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace game
{
	constexpr float touch_distance = 0.14f;

	Scene::Scene()
	{
		this->events.actor_hit.add_callback([this](ActorHitEvent e){this->on_actor_hit(e);});
		this->events.actor_struck.add_callback([this](ActorStruckEvent e){this->on_actor_struck(e);});
		this->events.actor_kill.add_callback([this](ActorKillEvent e){this->on_actor_kill(e);});
		this->events.actor_death.add_callback([this](ActorDeathEvent e){this->on_actor_death(e);});
		this->actors.reserve(QuadRenderer::max_quad_count);
	}

	void Scene::render()
	{
		HDK_PROFZONE("Scene - Render", 0xFF00AA00);
		hdk::assert(this->actors.size() == this->qrenderer.elements().size(), "Scene actor list (%zu) and QuadRenderer size (%zu) no longer match. Logic Error", this->actors.size(), this->qrenderer.elements().size());
		for(std::size_t i = 0; i < this->qrenderer.elements().size(); i++)
		{
			this->qrenderer.elements()[i].texture_id = this->get_actor(i).animation.get_texture();
		}
		this->qrenderer.render();
	}

	void Scene::update()
	{
		#if TZ_DEBUG
			this->debug_collision_query_count = 0;
		#endif
		HDK_PROFZONE("Scene - Update", 0xFF00AA00);
		this->quadtree.clear();
		for(std::size_t i = 0; i < this->size();)
		{

			Actor& actor = this->get_actor(i);
			actor.update();
			// Post update could theoretically kill it, so we must do that last.
			if(!this->actor_post_update(i))
			{
				this->update_quadtree(i++);
			}
		}
		this->update_camera();
		this->intersections = this->quadtree.find_all_intersections();
		this->collision_resolution();
	}
	
	void Scene::dbgui_current_scene()
	{
		ImGui::Text("Current Scene");
		ImGui::Spacing();
		if(ImGui::CollapsingHeader("Mass Control"))
		{
			ImGui::Indent();
			if(ImGui::Button("Kill Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.base_stats.current_health = 0;
				}
			}
			if(ImGui::Button("Resurrect Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.base_stats.current_health = actor.get_current_stats().max_health;
				}
			}
			if(ImGui::Button("Respawn Everyone"))
			{
				for(auto& actor : this->actors)
				{
					actor.respawn();
				}
			}
			if(ImGui::Button("Clear Scene"))
			{
				this->clear();
			}
			ImGui::Unindent();
		}
		if(ImGui::CollapsingHeader("Debug Spawning"))
		{
			ImGui::Indent();
			if(ImGui::Button("Debug Add Player"))
			{
				this->add(ActorType::PlayerClassic);
			}
			if(ImGui::Button("Debug Add Ghost Zombie"))
			{
				this->add(ActorType::GhostZombie);
			}
			if(ImGui::Button("Debug Add Ghost Banshee"))
			{
				this->add(ActorType::GhostBanshee);
			}
			if(ImGui::Button("Debug Add Ghost Zombie Spawner"))
			{
				this->add(ActorType::GhostZombie_Spawner);
			}
			ImGui::Unindent();
		}
		if(this->size() > 0 && ImGui::CollapsingHeader("Actors"))
		{
			ImGui::Indent();
			static int actor_id = 0;
			ImGui::SliderInt("Actor ID", &actor_id, 0, this->size() - 1);
			this->get_actor(actor_id).dbgui();
			ImGui::Unindent();
		}
		if(ImGui::CollapsingHeader("Quad Renderer"))
		{
			ImGui::Indent();
			this->qrenderer.dbgui();
			ImGui::Unindent();
		}
	}

	void Scene::dbgui_story_levels()
	{
		if(ImGui::Button("Empty Level"))
		{
			this->load_level(LevelID::Empty);
		}
		ImGui::SameLine();
		ImGui::Text("Nothin");
		if(ImGui::Button("DevLevel0"))
		{
			this->load_level(LevelID::DevLevel0);
		}
		ImGui::SameLine();
		ImGui::Text("An enclosed maze filled with the undead. Difficulty: Medium");
		if(ImGui::Button("DevLevel1"))
		{
			this->load_level(LevelID::DevLevel1);
		}
		ImGui::SameLine();
		ImGui::Text("A Banshee is consuming the souls of the innocent dead. Slay her. Difficulty: Low");
		if(ImGui::Button("DevLevel2"))
		{
			this->load_level(LevelID::DevLevel2);
		}
		ImGui::SameLine();
		ImGui::Text("An open arena with little cover. You are constantly barraged by fireballs. You will die. Difficulty: Very High");
	}

	void Scene::dbgui_procedural_level()
	{
		auto maybe_level_img = game::dbgui_generate_random_level_image();
		if(maybe_level_img.has_value())
		{
			this->impl_load_level(load_level_from_image(maybe_level_img.value()));
		}
	}

	std::size_t Scene::size() const
	{
		return this->qrenderer.elements().size();
	}

	bool Scene::empty() const
	{
		return this->size() == 0;
	}

	void Scene::add(ActorType type)
	{
		this->actors.push_back(game::create_actor(type));
		this->qrenderer.push();
		auto& actor = this->actors.back();
		if(actor.flags_new.has<FlagID::RandomSkin>())
		{
			const auto& flag = actor.flags_new.get<FlagID::RandomSkin>()->data();
			std::size_t skin_count = flag.skins.size();
			if(skin_count > 0)
			{
				std::uniform_int_distribution<std::size_t> dist{0, skin_count - 1};
				actor.skin = flag.skins[dist(this->rng)];
			}
		}
	}
	
	void Scene::pop()
	{
		this->qrenderer.pop();
		this->actors.pop_back();
	}

	void Scene::clear()
	{
		this->qrenderer.clear();
		this->actors.clear();
	}

	void Scene::load_level(LevelID level_id)
	{
		Level level = game::load_level(level_id);
		this->impl_load_level(level);
	}

	const Actor& Scene::get_actor(std::size_t id) const
	{
		hdk::assert(id < this->actors.size(), "Index out of range. Idx = %zu, size = %zu", id, this->actors.size());
		return this->actors[id];
	}

	Actor& Scene::get_actor(std::size_t id)
	{
		hdk::assert(id < this->actors.size(), "Index out of range. Idx = %zu, size = %zu", id, this->actors.size());
		return this->actors[id];
	}

	void Scene::impl_load_level(const Level& level)
	{
		// Firstly remove everything
		this->clear();
		// Add the player and her orb.
		this->add(ActorType::PlayerClassic);
		hdk::report("Player Spawns at {%.2f, %.2f}", level.player_spawn_location[0], level.player_spawn_location[1]);
		this->qrenderer.elements().front().position = level.player_spawn_location;
		// Now add actors from the level.
		for(const auto& [pos, actor_type] : level.actor_spawns)
		{
			this->add(actor_type);
			this->qrenderer.elements().back().position = pos;
		}
		this->level_boundaries = level.max_level_coords;
		auto& backdrop_data = this->qrenderer.backdrop();
		backdrop_data.texture_id = level.backdrop;
		backdrop_data.position = (this->level_boundaries / 2.0f) - hdk::vec2::filled(1.0f);
		backdrop_data.scale = ((this->level_boundaries - hdk::vec2::filled(0.25f)) / 2.0f);
		backdrop_data.layer = 0.999f;
	}

	hdk::vec2 Scene::get_mouse_position() const
	{
		return util::get_mouse_world_location() + this->qrenderer.camera_position();
	}

	void Scene::erase(std::size_t id)
	{
		this->qrenderer.erase(id);
		auto& this_actor = this->get_actor(id);
		this_actor.flags_new.clear();
		this_actor.entity.clear();
		std::swap(this_actor, this->actors.back());
		this->actors.pop_back();
	}

	bool Scene::actor_post_update(std::size_t id)
	{
		HDK_PROFZONE("Actor - Post Update", 0xFF8B4513);
		auto actor = [this, id]()->Actor&{return this->get_actor(id);};
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[id];
		quad.layer = static_cast<float>(actor().layer) / std::numeric_limits<unsigned short>::max();
		float touchdist = touch_distance;
		if(actor().flags_new.has<FlagID::ActionOnRepeat>())
		{
			auto& flag = actor().flags_new.get<FlagID::ActionOnRepeat>()->data();
			if(flag.current_time <= 0.0f)
			{
				if(flag.predicate == nullptr || (flag.predicate != nullptr && flag.predicate(actor())))
				{
					flag.current_time = flag.period;
					flag.actions.copy_components(actor().entity);
				}
			}
			else
			{
				unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - actor().last_update.millis<unsigned long long>();
				flag.current_time -= delta_millis;
			}
			
		}
		if(!this->is_in_bounds(id))
		{
			if(actor().flags_new.has<FlagID::ActionOnOOB>())
			{
				auto& flag = actor().flags_new.get<FlagID::ActionOnOOB>()->data();
				flag.actions.copy_components(actor().entity);
			}
		}
		if(!this->is_in_level(id))
		{
			if(actor().flags_new.has<FlagID::ActionOnOOL>())
			{
				auto& flag = actor().flags_new.get<FlagID::ActionOnOOL>()->data();
				flag.actions.copy_components(actor().entity);
			}
		}
		if(actor().flags_new.has<FlagID::Rot>())
		{
			this->do_actor_hit(Actor::NullID, id);
		}
		if(actor().flags_new.has<FlagID::CustomReach>())
		{
			touchdist *= actor().flags_new.get<FlagID::CustomReach>()->data().reach;
		}
		if(actor().flags_new.has<FlagID::AggressiveIf>() && !actor().dead())
		{
			auto& flag = actor().flags_new.get<FlagID::AggressiveIf>()->data();
			for(std::size_t i = 0; i < this->size(); i++)
			{
				if(i == id)
				{
					continue;
				}
				Actor& victim = this->get_actor(i);
				if(!victim.dead() && flag.predicate(actor(), victim) && !victim.flags_new.has<FlagID::Stealth>())
				{
					actor().entity.set<ActionID::GotoActor>
					({
						.actor_id = i
					});
					break;
				}
			}
		}
		if(actor().flags_new.has<FlagID::DieOnAnimationFinish>() && !actor().dead())
		{
			if(actor().animation.complete())
			{
				actor().base_stats.current_health = 0.0f;
			}
		}
		// If actor() wants to teleport to a random location, do it now.

		quad.scale = {0.2f, 0.2f};
		if(actor().flags_new.has<FlagID::InvisibleWhileDead>())
		{
			if(actor().dead())
			{
				quad.scale *= 0.0f;
			}
		}
		if(actor().flags_new.has<FlagID::CustomScale>())
		{
			auto scale = actor().flags_new.get<FlagID::CustomScale>()->data().scale;
			quad.scale[0] *= scale[0];
			quad.scale[1] *= scale[1];
		}

		// Handle actions.
		std::optional<hdk::vec2> chase_target = std::nullopt;
		// Entity Actions

		// Recursive Entity Actions (Can add other actions).
		SceneData data
		{
			.this_id = id,
			.mouse_position = this->get_mouse_position(),
			.chase_target = chase_target,
			.touch_distance = touchdist,
			.rng = this->rng,
			.get_living_players = [this](){return this->get_living_players();},
			.get_world_boundaries = [this](){return this->get_world_boundaries();},
			.spawn_actor = [this](ActorType t){this->add(t); return this->actors.size() - 1;},
			.get_actor = [this](std::size_t t)->Actor&{return this->get_actor(t);},
			.get_quad = [this](std::size_t t)->QuadRenderer::ElementData&{return this->qrenderer.elements()[t];}
		};

		auto handle_action = [this, &data]<ActionID ID>()
		{
			if(data.actor().entity.has<ID>())
			{
				game::action_invoke<ID>(data, *data.actor().entity.get<ID>());
			}
		};
		
		handle_action.template operator()<ActionID::RandomTeleport>();
		handle_action.template operator()<ActionID::GotoMouse>();
		handle_action.template operator()<ActionID::LaunchToMouse>();
		handle_action.template operator()<ActionID::LaunchToPlayer>();
		if(actor().entity.has<ActionID::TeleportToPlayer>())
		{
			auto maybe_player_id = this->find_first_player();
			if(maybe_player_id.has_value())
			{
				actor().entity.add<ActionID::Teleport>
				({
					.position = this->qrenderer.elements()[maybe_player_id.value()].position
				});
			}
			actor().entity.get<ActionID::TeleportToPlayer>()->set_is_complete(true);
		}
		handle_action.template operator()<ActionID::GotoPlayer>();
		handle_action.template operator()<ActionID::GotoActor>();
		// Concrete Entity Actions
		handle_action.template operator()<ActionID::GotoTarget>();
		handle_action.template operator()<ActionID::Launch>();
		handle_action.template operator()<ActionID::Teleport>();
		if(actor().entity.has<ActionID::HorizontalFlip>())
		{
			handle_action.template operator()<ActionID::HorizontalFlip>();
		}
		else
		{
			quad.scale[0] = std::abs(quad.scale[0]);
		}
		if(actor().entity.has<ActionID::VerticalFlip>())
		{
			handle_action.template operator()<ActionID::VerticalFlip>();
		}
		else
		{
			quad.scale[1] = std::abs(quad.scale[1]);
		}
		handle_action.template operator()<ActionID::SpawnActor>();
		handle_action.template operator()<ActionID::Respawn>();
		handle_action.template operator()<ActionID::RespawnAs>();
		handle_action.template operator()<ActionID::Despawn>();
		if(actor().entity.has<ActionID::Die>())
		{
			handle_action.template operator()<ActionID::Die>();
			this->on_actor_death({.killee = actor(), .killer = this->world_actor});
		}
		handle_action.template operator()<ActionID::ApplyBuff>();
		handle_action.template operator()<ActionID::ApplyBuffToActor>();
		handle_action.template operator()<ActionID::ApplyBuffToTarget>();
		handle_action.template operator()<ActionID::ApplyBuffToPlayers>();
		handle_action.template operator()<ActionID::DelayedAction>();
		handle_action.template operator()<ActionID::Cast>();

		// It's chasing something, but we don't care about what it's chasing.
		// if its not a player, we don't want it to move while it's casting though.
		const bool npc_is_casting = !actor().flags_new.has<FlagID::Player>() && actor().entity.has<ActionID::Cast>();
		if(npc_is_casting)
		{
			actor().motion = {};
		}
		if(chase_target.has_value() && !npc_is_casting)
		{
			actor().motion = {};
			// Get the displacement between the actor() and its chase target.
			const hdk::vec2 target_pos = chase_target.value();
			hdk::vec2 dist_to_target = target_pos - quad.position;
			// Find out which direction we need to go, or if we're already at the target.
			// So why can't we use touchdist here?
			// Let's say x is a ghost, and y is the player
			//
			//	    y
			//        / |
			//	x -- 
			//	If the distance between the two is less than or equal to touchdist, x starts hurting y.
			//	But, that is not the same than if the horizontal and vertical component of that vector are both touchdist.
			//	x^2 + y^2 = z^2, z = touchdist
			//	Thus touchdist^2 = x^2 + y^2
			//	Assume worst case x == y:
			//	touchdist^2 = 2x^2
			//	x^2 = (touchdist^2)/2
			//	// x = sqrt((touchdist^2) / 2)
			float sqrt_dist = std::sqrt(std::pow(touchdist, 2) / 2.0f);
			if(dist_to_target[0] > sqrt_dist)
			{
				// We need to move right.
				actor().motion |= ActorMotion::MoveRight;
			}
			else if(dist_to_target[0] < -sqrt_dist)
			{
				// We need to move left.
				actor().motion |= ActorMotion::MoveLeft;
			}

			if(dist_to_target[1] > sqrt_dist)
			{
				// We need to move upwards.
				actor().motion |= ActorMotion::MoveUp;
			}
			else if(dist_to_target[1] < -sqrt_dist)
			{
				actor().motion |= ActorMotion::MoveDown;
			}
		}
		else
		{
			// Actor isn't currently chasing something.
			if(actor().flags_new.has<FlagID::WanderIfIdle>())
			{
				const auto& flag = actor().flags_new.get<FlagID::WanderIfIdle>()->data();
				std::uniform_real_distribution<float> dist{0.0f, 1.0f};
				if(dist(this->rng) < flag.wander_chance)
				{
					std::uniform_real_distribution<float> dist2(flag.max_wander_range * -0.5f, flag.max_wander_range * 0.5f);
					// Set an action to wander to a nearby location.
					hdk::vec2 target_loc = quad.position;
					target_loc += hdk::vec2{dist2(this->rng), dist2(this->rng)};
					actor().entity.add<ActionID::GotoTarget>
					({
						.target_position = target_loc
					});
				}
				
			}
		}
		// We now know for certain whether the actor() wants to move or not. Now we can finally carry out the movement.
		if(!actor().dead())
		{
			float sp = actor().get_current_stats().movement_speed;
			hdk::vec2 position_change{0.0f, 0.0f};
			if(actor().motion.contains(ActorMotion::MoveLeft))
			{
				position_change[0] -= 1;
			}
			if(actor().motion.contains(ActorMotion::MoveRight))
			{
				position_change[0] += 1;
			}
			if(actor().motion.contains(ActorMotion::MoveUp))
			{
				position_change[1] += 1;
			}
			if(actor().motion.contains(ActorMotion::MoveDown))
			{
				position_change[1] -= 1;
			}
			unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - actor().last_update.millis<unsigned long long>();
			quad.position += position_change.normalised() * sp * static_cast<float>(delta_millis) / 5.0f;
		}
		this->update_status_events(id);
		actor().last_update = tz::system_time();
		return this->garbage_collect(id);
	}

	std::vector<std::size_t> Scene::get_living_players() const
	{
		HDK_PROFZONE("Scene - Query Living Players", 0xFF00AA00);
		std::vector<std::size_t> ret;
		for(std::size_t i = 0; i < this->size(); i++)
		{
			
			if(this->get_actor(i).flags_new.has<FlagID::Player>() && !this->get_actor(i).dead())
			{
				ret.push_back(i);
			}
		}
		return ret;
	}

	std::optional<std::size_t> Scene::find_first_player() const
	{
		HDK_PROFZONE("Scene - Find First Player", 0xFF00AA00);
		auto players = this->get_living_players();
		if(!players.empty())
		{
			return players.front();
		}
		return std::nullopt;
	}

	Box Scene::get_bounding_box(std::size_t actor_id) const
	{
		const QuadRenderer::ElementData& quad = this->qrenderer.elements()[actor_id];
		hdk::vec2 scale = quad.scale;
		// Bounding box should be affected by custom reach.
		const Actor& actor = this->get_actor(actor_id);
		if(actor.flags_new.has<FlagID::CustomReach>())
		{
			const auto& flag = actor.flags_new.get<FlagID::CustomReach>()->data();
			scale *= flag.reach;
		}
		hdk::vec2 min{-0.5f, -0.5f};
		hdk::vec2 max{0.5f, 0.5f};
		scale[0] = std::abs(scale[0]);
		scale[1] = std::abs(scale[0]);
		min[0] *= scale[0];
		min[1] *= scale[1];
		max[0] *= scale[0];
		max[1] *= scale[1];
		min += quad.position;
		max += quad.position;
		return {min, max};
	}

	bool Scene::actor_collision_query(std::size_t actor_a, std::size_t actor_b) const
	{
		#if TZ_DEBUG
			this->debug_collision_query_count++;
		#endif
		HDK_PROFZONE("Scene - Collision Query", 0xFF00AA00);
		QuadtreeNode a_node{.actor_id = actor_a};
		QuadtreeNode b_node{.actor_id = actor_b};
		return std::find_if(this->intersections.begin(), this->intersections.end(), [&a_node, b_node](const auto& pair)
		{
			return (pair.first == a_node && pair.second == b_node)
			    || (pair.first == b_node && pair.second == a_node);
		}) != this->intersections.end();
	}

	bool Scene::is_in_bounds(std::size_t actor_id) const
	{
		HDK_PROFZONE("Scene - Bounds Check", 0xFF00AA00);
		hdk::vec2 pos = this->qrenderer.elements()[actor_id].position;
		auto bounds = this->get_world_boundaries();
		return
			bounds.first[0] <= pos[0] && pos[0] <= bounds.second[0]
		     && bounds.first[1] <= pos[1] && pos[1] <= bounds.second[1];
	}

	bool Scene::is_in_level(std::size_t actor_id) const
	{
		HDK_PROFZONE("Scene - Level Bounds Check", 0xFF00AA00);
		hdk::vec2 pos = this->qrenderer.elements()[actor_id].position;
		hdk::vec2 level_min = this->level_boundaries * -1.0f;
		return
			level_min[0] <= pos[0] && pos[0] <= this->level_boundaries[0]
		     && level_min[1] <= pos[1] && pos[1] <= this->level_boundaries[1];
	}

	std::pair<hdk::vec2, hdk::vec2> Scene::get_world_boundaries() const
	{
		const float width_mod = this->qrenderer.get_width_multiplier();
		const hdk::vec2 camera_pos = this->qrenderer.camera_position();
		return
		{
			hdk::vec2{-width_mod, -1.0f} + camera_pos, hdk::vec2{width_mod, 1.0f} + camera_pos
		};
	}

	void Scene::update_camera()
	{
		HDK_PROFZONE("Scene - Camera Update", 0xFF00AA00);
		auto player_ids = this->get_living_players();
		std::vector<std::size_t> actors_to_view;
		if(player_ids.empty())
		{
			actors_to_view.resize(this->size());
			std::iota(actors_to_view.begin(), actors_to_view.end(), 0ull);
		}
		else
		{
			actors_to_view = {player_ids};
		}

		constexpr float fmax = std::numeric_limits<float>::max();
		constexpr float fmin = std::numeric_limits<float>::min();
		hdk::vec2 min{fmax, fmax}, max{fmin, fmin}, avg{0.0f, 0.0f};
		for(std::size_t actor_id : actors_to_view)
		{
			hdk::vec2 pos = this->qrenderer.elements()[actor_id].position;
			min[0] = std::min(min[0], pos[0]);
			min[1] = std::min(min[1], pos[1]);
			max[0] = std::max(max[0], pos[0]);
			max[1] = std::max(max[1], pos[1]);
			avg[0] += pos[0];
			avg[1] += pos[1];
		}
		avg /= static_cast<float>(actors_to_view.size());
		// We want to view 'avg'. If it's far enough from the camera by some constant we will start to move towards it.
		const hdk::vec2 camera_displacement = avg - this->qrenderer.camera_position();
		if(camera_displacement.length() > 0.5f)
		{
			
#if 1
			this->qrenderer.camera_position() += camera_displacement * 0.02f * camera_displacement.length();
#else
			this->qrenderer.camera_position() = avg;
#endif
		}
	}

	void Scene::update_status_events(std::size_t id)
	{
		HDK_PROFZONE("Scene - Status Events Update", 0xFF00AA00);
		const Actor& actor = this->get_actor(id);
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[id];
		int status_effect = StatusEffect_None;
		if(actor.buffs.contains(BuffID::Berserk))
		{
			status_effect = StatusEffect_Enrage;
		}
		if(actor.buffs.contains(BuffID::Chill))
		{
			status_effect = StatusEffect_Cold;
		}
		quad.status_effect_id = status_effect;
	}

	void Scene::update_quadtree(std::size_t actor_id)
	{
		HDK_PROFZONE("Scene - Quadtree Update", 0xFF00AA00);
		this->quadtree.add({.actor_id = actor_id, .bounding_box = this->get_bounding_box(actor_id)});
	}

	bool Scene::garbage_collect(std::size_t id)
	{
		HDK_PROFZONE("Scene - Garbage Collect", 0xFF00AA00);
		// Erase means to swap with the last and then pop it back
		const bool dead = this->get_actor(id).dead();
		if(dead)
		{
			auto iter = this->despawn_timer.find(id);
			if(iter != this->despawn_timer.end())
			{
				// It has an entry, see if its timed out.
				if(iter->second.done() && !this->get_actor(id).flags_new.has<FlagID::DoNotGarbageCollect>())
				{
					// Timed out. we purge.
					// Erase will swap us with the last element and then kill last element (i.e us).
					// So if last element has a despawn timer aswell it must become id.
					auto last_id = this->size() - 1;
					if(this->despawn_timer.find(last_id) != this->despawn_timer.end())
					{
						iter->second = this->despawn_timer.at(last_id);
						this->despawn_timer.erase(last_id);
					}
					this->erase(id);
					return true;
				}
			}
			else
			{
				using namespace tz::literals;
				// Actor either despawns in 45s, or a custom value.
				if(this->get_actor(id).flags_new.has<FlagID::CustomGarbageCollectPeriod>())
				{
					tz::Duration d{this->get_actor(id).flags_new.get<FlagID::CustomGarbageCollectPeriod>()->data().delay_millis};
					this->despawn_timer.emplace(id, d);
				}
				else
				{
					this->despawn_timer.emplace(id, 45_s);
				}
			}
		}
		else
		{
			this->despawn_timer.erase(id);
		}
		return false;
	}

	void Scene::collision_resolution()
	{
		HDK_PROFZONE("Scene - Collision Resolution", 0xFF00AA00);
		for(const auto& [node_a, node_b] : this->intersections)
		{
			std::size_t a_id = node_a.actor_id;
			std::size_t b_id = node_b.actor_id;

			this->resolve_collision(a_id, b_id);
			this->resolve_collision(b_id, a_id);
		}
	}

	void Scene::resolve_collision(std::size_t a_id, std::size_t b_id)
	{
		HDK_PROFZONE("Scene - Single Collision Resolution", 0xFF00AA00);
		Actor& actor = this->get_actor(a_id);
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[a_id];
		Actor& other = this->get_actor(b_id);
		QuadRenderer::ElementData& other_quad = this->qrenderer.elements()[b_id];

		bool is_hazardous = actor.flags_new.has<FlagID::HazardousIf>();
		if(is_hazardous)
		{
			const auto& flag = actor.flags_new.get<FlagID::HazardousIf>()->data();
			is_hazardous = flag.predicate(actor, other);
		}

		const bool wants_to_hurt = (is_hazardous) && actor.is_enemy_of(other) && !other.flags_new.has<FlagID::Unhittable>() && !actor.dead() && !other.dead();
		const bool blocks_colliders = actor.flags_new.has<FlagID::Collide>() &&
			!actor.dead() && !other.dead() &&
			(actor.flags_new.get<FlagID::Collide>()->data().collision_filter.contains(other.type) || actor.flags_new.get<FlagID::Collide>()->data().collision_filter.empty()) && !actor.flags_new.get<FlagID::Collide>()->data().collision_blacklist.contains(other.type);
		bool wants_touch_other = false;
		if(actor.flags_new.has<FlagID::ActionOnActorTouch>())
		{
			auto flag = actor.flags_new.get<FlagID::ActionOnActorTouch>();
			if(flag->data().allow_dead || !other.dead())
			{
				wants_touch_other = flag->data().predicate(actor, other);
			}
		}
		const bool cares_about_collisions = wants_to_hurt || blocks_colliders || wants_touch_other;
		if(cares_about_collisions)
		{
			if(wants_to_hurt)
			{
				this->do_actor_hit(a_id, b_id);
			}
			if(blocks_colliders)
			{
				hdk::vec2 displacement = quad.position - other_quad.position;
				// Push back with impulse 150% the distance it would've travelled this frame.
				other_quad.position -= displacement.normalised() * other.get_current_stats().movement_speed * 1.5f * actor.density;
			}
			if(wants_touch_other)
			{
				// Actor is touching the subject, and it wants to.
				// Carry out its actions.
				auto& on_touch_flag = actor.flags_new.get<FlagID::ActionOnActorTouch>()->data();
				on_touch_flag.actions.copy_components(actor.entity);
			}
		}
	}

	void Scene::do_actor_hit(Actor& a, Actor& b)
	{
		bool alive = !b.dead();
		a.damage(b);
		this->events.actor_hit({.attacker = a, .attackee = b});
		this->events.actor_struck({.attackee = b, .attacker = a});
		if(alive && b.dead())
		{
			this->events.actor_kill({.killer = a, .killee = b});
			this->events.actor_death({.killee = b, .killer = a});
		}
	}

	void Scene::do_actor_hit(std::size_t attacker, std::size_t attackee)
	{
		Actor* aptr = nullptr;
		if(attacker == Actor::NullID)
		{
			aptr = &this->world_actor;
		}
		else
		{
			aptr = &this->get_actor(attacker);
		}
		Actor& a = *aptr;
		Actor& b = this->get_actor(attackee);
		this->do_actor_hit(a, b);
	}

	void Scene::on_actor_hit(ActorHitEvent e)
	{
		HDK_PROFZONE("Scene - On Actor Hit", 0xFF8B4513);
		e.attacker.target = &e.attackee;
		if(e.attacker.flags_new.has<FlagID::ActionOnHit>())
		{
			auto& flag = e.attacker.flags_new.get<FlagID::ActionOnHit>()->data();
			flag.actions.copy_components(e.attacker.entity);
		}
		if(e.attacker.flags_new.has<FlagID::SelfRecoil>())
		{
			this->do_actor_hit(this->world_actor, e.attacker);
		}
	}

	void Scene::on_actor_struck(ActorStruckEvent e)
	{
		HDK_PROFZONE("Scene - On Actor Struck", 0xFF8B4513);
		//hdk::report("%s struck by %s", e.attackee.name, e.attacker.name);
		if(e.attackee.flags_new.has<FlagID::ActionOnStruck>())
		{
			auto& flag = e.attackee.flags_new.get<FlagID::ActionOnStruck>()->data();
			if(flag.internal_cooldown > 0.0f)
			{
				flag.internal_cooldown -= (tz::system_time() - e.attackee.last_update).millis<float>();
			}
			else
			{
				flag.internal_cooldown = flag.icd;
				flag.actions.copy_components(e.attackee.entity);
			}
		}
	}

	void Scene::on_actor_kill(ActorKillEvent e)
	{
		HDK_PROFZONE("Scene - On Actor Kill", 0xFF8B4513);
		//hdk::report("%s killed %s", e.killer.name, e.killee.name);
	}

	void Scene::on_actor_death(ActorDeathEvent e)
	{
		HDK_PROFZONE("Scene - On Actor Death", 0xFF8B4513);
		// If something dies, it's not moving anymore.
		e.killee.motion = {};
		//hdk::report("%s killed by %s", e.killee.name, e.killer.name);
		if(e.killee.flags_new.has<FlagID::ActionOnDeath>())
		{
			auto& flag = e.killee.flags_new.get<FlagID::ActionOnDeath>()->data();
			flag.actions.copy_components(e.killee.entity);
		}
		if(e.killee.flags_new.has<FlagID::RespawnOnDeath>())
		{
			e.killee.entity.add<ActionID::Respawn>();
		}
	}
}

