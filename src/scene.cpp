#include "scene.hpp"
#include "util.hpp"
#include "effects/light.tzsl"
#include "tz/core/job/job.hpp"
#include "tz/core/debug.hpp"
#include "tz/core/profile.hpp"

#include <cctype>
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

		#if TZ_DEBUG
			this->game_bar_dbgui_callback = tz::dbgui::game_bar().add_callback([this]()
			{
				ImGui::Text("%s Z%zu |", this->zone.name, this->zone.level_cursor);
				ImGui::SameLine();
				auto fact_count = [this](Faction f)
				{
					return std::count_if(this->actors.begin(), this->actors.end(), [f](const Actor& a)
					{
						return a.faction == f && !a.dead() && a.flags.has<FlagID::StatTracked>();
					});
				};
				auto flag_count = [this]<FlagID ID>(bool want_alive)
				{
					return std::count_if(this->actors.begin(), this->actors.end(), [want_alive](const Actor& a)
					{
						bool ret = a.flags.has<ID>();
						if(want_alive) ret &= !a.dead();
						return ret;
					});
				};

				const std::size_t total_count = std::count_if(this->actors.begin(), this->actors.end(), [](const Actor& a){return !a.dead() && a.flags.has<FlagID::StatTracked>();});
				const std::size_t total_actor_count = this->actors.size();
				ImGui::Text("Factions: D:%zu, PF:%zu, PE:%zu, F:%zu, E:%zu, Tot:%zu, AC:%zu | Up:%zu, Down:%zu/%zu |",
				fact_count(Faction::Default),
				fact_count(Faction::PlayerFriend),
				fact_count(Faction::PlayerEnemy),
				fact_count(Faction::PureFriend),
				fact_count(Faction::PureEnemy),
				total_count,
				total_actor_count,
				flag_count.operator()<FlagID::GoesUpALevel>(false),
				flag_count.operator()<FlagID::GoesDownALevel>(true),
				flag_count.operator()<FlagID::GoesDownALevel>(false)
				);
			});
		#endif // TZ_DEBUG
	}

	Scene::~Scene()
	{
		#if TZ_DEBUG
			tz::dbgui::game_bar().remove_callback(this->game_bar_dbgui_callback);
			this->game_bar_dbgui_callback = tz::nullhand;
		#endif // TZ_DEBUG
	}

	void Scene::render()
	{
		TZ_PROFZONE("Scene - Render", 0xFF00AA00);

		tz::assert(this->actors.size() == this->qrenderer.elements().size(), "Scene actor list (%zu) and QuadRenderer size (%zu) no longer match. Logic Error", this->actors.size(), this->qrenderer.elements().size());
		game::effects().update({this->qrenderer.get_effect(0), this->qrenderer.get_effect(1)});
		for(std::size_t i = 0; i < this->qrenderer.elements().size(); i++)
		{
			TextureID tex = this->get_actor(i).animation.get_texture();
			this->qrenderer.elements()[i].texture_id = tex;
			this->qrenderer.elements()[i].normal_map_id = game::get_normal_map_for(tex);
		}
		this->qrenderer.render();
	}

	void Scene::update()
	{
		#if TZ_DEBUG
			this->debug_collision_query_count = 0;
		#endif
		TZ_PROFZONE("Scene - Update", 0xFF00AA00);
		this->quadtree.clear();
		this->impl_light_actor_count = 0;
		for(std::size_t i = 0; i < max_light_count; i++)
		{
			game::effects().point_lights()[i] = {};
		}
		for(std::size_t i = 0; i < this->size();)
		{

			Actor& actor = this->get_actor(i);
			actor.update();
			// Post update could theoretically kill it, so we must do that last.
			auto res = this->actor_post_update(i);
			switch(res)
			{
				case ActorPostUpdateResult::Typical:
					this->update_quadtree(i++);
				break;
				case ActorPostUpdateResult::ActorDeleted:
				break;
				case ActorPostUpdateResult::LevelDeleted:
					return;
				break;
			}
		}
		this->update_camera();
		this->intersections = this->quadtree.find_all_intersections();
		this->collision_resolution();
	}

	constexpr auto c_actor_count = static_cast<std::size_t>(ActorType::Count);

	std::array<const char*, c_actor_count> get_actor_names()
	{
		std::array<const char*, c_actor_count> ret;
		for(std::size_t i = 0; i < c_actor_count; i++)
		{
			ret[i] = game::create_actor(static_cast<ActorType>(i)).name;
		}
		return ret;
	}
	
	void Scene::dbgui_current_scene()
	{
		static std::array<const char*, c_actor_count> actor_names = get_actor_names();
		ImGui::Text("Current Scene");
		ImGui::Spacing();
		if(ImGui::CollapsingHeader("Track Actor"))
		{
			static std::array<char, 128> name_search_buf = {'\0'};
			static std::size_t tracked_uuid;

			auto name_match = [](const char* lhs, const char* rhs)->bool
			{
				std::string lhs_s{lhs};
				std::string rhs_s{rhs};
				std::transform(lhs_s.begin(), lhs_s.end(), lhs_s.begin(), ::toupper);
				std::transform(rhs_s.begin(), rhs_s.end(), rhs_s.begin(), ::toupper);
				return lhs_s == rhs_s;
			};

			std::optional<ActorType> find_match = std::nullopt;
			ImGui::InputText("Search by name", name_search_buf.data(), name_search_buf.size());
			if(ImGui::BeginCombo("Or search by type", "Actor Type Name..."))
			{
				tracked_uuid = Actor::NullID;
				for(int i = 0; i < actor_names.size(); i++)
				{
					bool is_selected = name_match(name_search_buf.data(), actor_names[i]);	
					if(ImGui::Selectable(actor_names[i], is_selected))
					{
						std::strcpy(name_search_buf.data(), actor_names[i]);
					}
					if(is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			{
				std::string lhs{name_search_buf.data()};
				for(std::size_t i = 0; i < actor_names.size(); i++)
				{
					if(name_match(name_search_buf.data(), actor_names[i]))
					{
						find_match = static_cast<ActorType>(i);
						break;
					}
				}
			}
			std::size_t aid = this->get_actor_from_uuid(tracked_uuid);
			Actor* a = nullptr;
			if(aid != Actor::NullID)
			{
				a = &this->get_actor(aid);
			}
			ImGui::Indent();
			if(a != nullptr)
			{
				a->dbgui();
			}
			else
			{
				ImGui::Text("Actor by UUID %d cannot be found.", tracked_uuid);
			}
			ImGui::Unindent();
			if(find_match.has_value())
			{
				for(const Actor& a : this->actors)
				{
					if(a.type == find_match.value())
					{
						tracked_uuid = a.uuid;
					}
				}
			}
			else
			{
				ImGui::Text("No actor name matching \"%s\"", name_search_buf.data());
				tracked_uuid = Actor::NullID;
			}
		}
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
			if(ImGui::Button("Kill Non-Players"))
			{
				for(auto& actor : this->actors)
				{
					if(!actor.flags.has<FlagID::Player>())
					{
						actor.base_stats.current_health = 0;
					}
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
				this->add(ActorType::PlayerAkhara);
			}
			if(ImGui::Button("Debug Add Ghost Zombie"))
			{
				this->add(ActorType::GhostZombie);
			}
			if(ImGui::Button("Debug Add Ghost Banshee"))
			{
				this->add(ActorType::GhostBanshee_Skittish);
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
		if(ImGui::Button("Next Level"))
		{
			this->impl_next_level();
		}
		ImGui::Text("Story Levels (Zones)");
		if(ImGui::Button("Story Level 1: Blanchfield Banshee"))
		{
			this->load_zone(StoryZone::BlanchfieldGraveyard);
		}
		ImGui::Text("Legacy Standalone Levels (Deprecated)");
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
		ImGui::TextWrapped("An enclosed maze filled with the undead. Difficulty: Medium");
		if(ImGui::Button("DevLevel1"))
		{
			this->load_level(LevelID::DevLevel1);
		}
		ImGui::SameLine();
		ImGui::TextWrapped("A Banshee haunts the mausoleum of Blanchfield Graveyard, forcing the dead to rise again. Destroy the graves to deprive her of souls, and then destroy her. Difficulty: High");
		if(ImGui::Button("DevLevel2"))
		{
			this->load_level(LevelID::DevLevel2);
		}
		ImGui::SameLine();
		ImGui::TextWrapped("You are constantly bombarded. Your shelter would do nicely, if it weren't being invaded by the undead... Difficulty: Medium");
		if(ImGui::Button("DevLevel3"))
		{
			this->load_level(LevelID::DevLevel3);
		}
		ImGui::SameLine();
		ImGui::TextWrapped("Return to Blanchfield Graveyard during the winter.");
	}

	void Scene::dbgui_procedural_level()
	{
		auto maybe_level_info = game::dbgui_procedural_zones();
		if(maybe_level_info.has_value())
		{
			this->impl_load_zone(game::get_procedural_zone(maybe_level_info.value()));
			this->qrenderer.elements().front().position = this->level_boundaries / 2.0f;
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
		if(actor.flags.has<FlagID::RandomSkin>())
		{
			const auto& flag = actor.flags.get<FlagID::RandomSkin>()->data();
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

	void Scene::load_zone(StoryZone zone)
	{
		this->impl_load_zone(game::get_story_zone(zone));
	}

	void Scene::load_level(LevelID level_id)
	{
		Level level = game::load_level(level_id);
		this->impl_load_level(level);
	}

	const Actor& Scene::get_actor(std::size_t id) const
	{
		tz::assert(id < this->actors.size(), "Index out of range. Idx = %zu, size = %zu", id, this->actors.size());
		return this->actors[id];
	}

	Actor& Scene::get_actor(std::size_t id)
	{
		tz::assert(id < this->actors.size(), "Index out of range. Idx = %zu, size = %zu", id, this->actors.size());
		return this->actors[id];
	}

	void Scene::impl_load_zone(Zone z)
	{
		this->zone = z;
		tz::assert(!this->zone.levels.empty(), "Current zone has no levels.");
		tz::assert(this->zone.level_cursor < this->zone.levels.size(), "Current zone level cursor (%zu) is not valid for the number of levels of the zone (%zu).", this->zone.level_cursor, this->zone.levels.size());
		this->impl_load_level(this->zone.levels[this->zone.level_cursor]);
		// Each level has a spawn point for the player. However, we have a special case if the level is the initial level, where the level might have a specific spawn point for the first time play. In this case, we use that.
		if(this->zone.initial_load)
		{
			this->qrenderer.elements().front().position = this->zone.initial_spawn;
		}
	}

	void Scene::impl_load_level(const Level& level)
	{
		// Firstly remove everything
		this->clear();
		// Reset zoom.
		this->qrenderer.set_camera_zoom(1.0f);
		// Add the player and her orb.
		this->add(ActorType::PlayerAkhara);
		tz::report("Player Spawns at {%.2f, %.2f}", level.player_spawn_location[0], level.player_spawn_location[1]);
		this->qrenderer.elements().front().position = level.player_spawn_location;
		this->qrenderer.set_ambient_lighting(level.ambient_lighting);
		// Now add actors from the level.
		for(const auto& [pos, actor_type] : level.actor_spawns)
		{
			this->add(actor_type);
			this->qrenderer.elements().back().position = pos;
		}
		// Quadtree boundary is equal to level boundary with an extra 1/8th border.
		tz::vec2 quadtree_boundary = level.max_level_coords * 1.125f;
		this->quadtree = Quadtree<QuadtreeNode>{Box{quadtree_boundary * -1.0f, quadtree_boundary}};

		this->level_boundaries = level.max_level_coords;

		const tz::vec2 backdrop_position = this->level_boundaries / 2.0f;
		const tz::vec2 backdrop_scale = (this->level_boundaries) / 2.0f;

		auto& background_data = this->qrenderer.overlay(OverlayID::Backdrop_Background);
		background_data.texture_id = level.backdrop.background;
		background_data.normal_map_id = game::get_normal_map_for(background_data.texture_id);
		background_data.position = backdrop_position;
		background_data.scale = backdrop_scale;
		background_data.layer = -1.0f;
		// boundaries are proportional to the level image dimensions.
		// the background texcoord scale should be lower if the level image is really big.
		const tz::vec2 texcoord_scale{std::cbrt(this->level_boundaries[0]), std::cbrt(this->level_boundaries[1])};

		background_data.texcoord_scale = texcoord_scale;
		auto& foreground_data = this->qrenderer.overlay(OverlayID::Backdrop_Foreground);
		foreground_data.texture_id = level.backdrop.foreground;
		foreground_data.normal_map_id = game::get_normal_map_for(foreground_data.texture_id);
		foreground_data.position = backdrop_position;
		foreground_data.scale = backdrop_scale;
		foreground_data.layer = -0.9f;

		auto handle_effect = [this, backdrop_position, backdrop_scale, texcoord_scale](EffectID id, std::size_t effect_number)
		{
			auto oid = static_cast<OverlayID>(static_cast<int>(OverlayID::Effect) + effect_number);
			auto& effect_data = this->qrenderer.overlay(oid);
			effect_data.position = backdrop_position;
			effect_data.layer = 0.0f + (effect_number * 0.01f);
			
			this->qrenderer.set_effect(id, effect_number);
			effect_data.scale = (id != EffectID::None)
				? backdrop_scale
				: tz::vec2::zero();
			if(id == EffectID::LightLayer)
			{
				effect_data.texcoord_scale = this->level_boundaries / this->level_boundaries[0];
			}
			else
			{
				effect_data.texcoord_scale = texcoord_scale;
			}
		};
		handle_effect(level.weather_effect, 0);
		handle_effect(EffectID::LightLayer, 1);
		game::effects().notify_level_dimensions(this->level_boundaries);
	}

	void Scene::impl_next_level()
	{
		this->zone.initial_load = false;
		tz::assert(this->zone.level_cursor + 1 < this->zone.levels.size(), "No next level exists.");
		this->impl_load_level(this->zone.levels[++this->zone.level_cursor]);
	}

	void Scene::impl_prev_level()
	{
		this->zone.initial_load = false;
		tz::assert(this->zone.level_cursor > 0, "No previous level exists.");
		this->impl_load_level(this->zone.levels[--this->zone.level_cursor]);
	}

	tz::vec2 Scene::get_mouse_position() const
	{
		return (util::get_mouse_world_location() * this->qrenderer.get_camera_zoom()) + this->qrenderer.camera_position();
	}

	void Scene::erase(std::size_t id)
	{
		this->qrenderer.erase(id);
		auto& this_actor = this->get_actor(id);
		this_actor.flags.clear();
		this_actor.entity.clear();
		std::swap(this_actor, this->actors.back());
		this->actors.pop_back();
	}

	ActorPostUpdateResult Scene::actor_post_update(std::size_t id)
	{
		TZ_PROFZONE("Actor - Post Update", 0xFF8B4513);
		auto actor = [this, id]()->Actor&{return this->get_actor(id);};
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[id];
		quad.layer = static_cast<float>(actor().layer) / std::numeric_limits<unsigned short>::max();
		float touchdist = touch_distance;
		if(actor().flags.has<FlagID::ActionOnRepeat>())
		{
			auto& flag = actor().flags.get<FlagID::ActionOnRepeat>()->data();
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
			if(actor().flags.has<FlagID::ActionOnOOB>())
			{
				auto& flag = actor().flags.get<FlagID::ActionOnOOB>()->data();
				flag.actions.copy_components(actor().entity);
			}
		}
		if(!this->is_in_level(id))
		{
			if(actor().flags.has<FlagID::ActionOnOOL>())
			{
				auto& flag = actor().flags.get<FlagID::ActionOnOOL>()->data();
				flag.actions.copy_components(actor().entity);
			}
		}
		if(actor().flags.has<FlagID::Rot>())
		{
			this->do_actor_hit(Actor::NullID, id);
		}
		if(actor().flags.has<FlagID::CustomReach>())
		{
			touchdist *= actor().flags.get<FlagID::CustomReach>()->data().reach;
		}
		if(actor().flags.has<FlagID::AggressiveIf>() && !actor().dead())
		{
			auto& flag = actor().flags.get<FlagID::AggressiveIf>()->data();
			for(std::size_t i = 0; i < this->size(); i++)
			{
				if(i == id)
				{
					continue;
				}
				Actor& victim = this->get_actor(i);
				if(flag.predicate(actor(), victim) && !victim.flags.has<FlagID::Stealth>())
				{
					actor().entity.set<ActionID::GotoActor>
					({
						.actor_id = i
					});
					break;
				}
			}
		}
		if(actor().flags.has<FlagID::DieOnAnimationFinish>() && !actor().dead())
		{
			if(actor().animation.complete())
			{
				actor().base_stats.current_health = 0.0f;
			}
		}
		// If actor() wants to teleport to a random location, do it now.

		quad.scale = {0.2f, 0.2f};
		if(actor().flags.has<FlagID::InvisibleWhileDead>())
		{
			if(actor().dead())
			{
				quad.scale *= 0.0f;
			}
		}
		if(actor().flags.has<FlagID::CustomScale>())
		{
			auto scale = actor().flags.get<FlagID::CustomScale>()->data().scale;
			quad.scale[0] *= scale[0];
			quad.scale[1] *= scale[1];
		}
		
		if(actor().flags.has<FlagID::Light>())
		{
			if(this->impl_light_actor_count < game::effects().point_lights().size() - 1)
			{
				auto& flag = actor().flags.get<FlagID::Light>()->data();
				unsigned long long delta_millis = tz::system_time().millis<unsigned long long>() - actor().last_update.millis<unsigned long long>();
				flag.time += delta_millis;
				if(!actor().dead() || !actor().flags.has<FlagID::InvisibleWhileDead>())
				{
					auto l = flag.light;
					l.position = quad.position + flag.offset;
					if(actor().flags.has<FlagID::CustomScale>())
					{
						auto scale = actor().flags.get<FlagID::CustomScale>()->data().scale;
						float diff = (((scale[0] + scale[1]) * 0.5f - 0.5f) * 12.0f);
						if(diff <= 0.0f)
						{
							diff = 1.0f;
						}
						l.power *= diff;
					}
					l.power += std::clamp(std::sin(flag.time * 0.001f * flag.variance_rate), flag.min_variance_pct, flag.max_variance_pct) * flag.power_variance;
					if(flag.power_scale_with_health_pct)
					{
						auto stats = actor().get_current_stats();
						l.power *= stats.current_health / stats.max_health;
					}
					game::effects().point_lights()[++this->impl_light_actor_count] = l;
				}
			}
		}

		// Handle actions.
		std::optional<tz::vec2> chase_target = std::nullopt;
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
			.get_quad = [this](std::size_t t)->QuadRenderer::ElementData&{return this->qrenderer.elements()[t];},
			.get_actor_from_uuid = [this](std::size_t uuid){return this->get_actor_from_uuid(uuid);},
			.next_level = [this](){this->impl_next_level();},
			.previous_level = [this](){this->impl_prev_level();},
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
		handle_action.template operator()<ActionID::LaunchRandomDirection>();
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
		handle_action.template operator()<ActionID::MoveRelative>();
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
		handle_action.template operator()<ActionID::ApplyFlag>();
		if(actor().entity.has<ActionID::NextLevel>())
		{
			handle_action.template operator()<ActionID::NextLevel>();
			return ActorPostUpdateResult::LevelDeleted;
		}
		if(actor().entity.has<ActionID::PreviousLevel>())
		{
			handle_action.template operator()<ActionID::PreviousLevel>();
			return ActorPostUpdateResult::LevelDeleted;
		}

		// It's chasing something, but we don't care about what it's chasing.
		// if its not a player, we don't want it to move while it's casting though.
		const bool npc_is_casting = !actor().flags.has<FlagID::Player>() && actor().entity.has<ActionID::Cast>();
		const bool is_attached = actor().flags.has<FlagID::AttachedToActor>();
		if(npc_is_casting || is_attached)
		{
			actor().motion = {};
		}
		if(is_attached)
		{
			std::size_t parent_uuid = actor().flags.get<FlagID::AttachedToActor>()->data().actor_uuid;
			std::size_t parent = this->get_actor_from_uuid(parent_uuid);
			if(parent == Actor::NullID)
			{
				actor().flags.remove<FlagID::AttachedToActor>();
			}
			else
			{
			quad.position = this->qrenderer.elements()[parent].position;
			}
		}
		if(chase_target.has_value() && !npc_is_casting)
		{
			actor().motion = {};
			// Get the displacement between the actor() and its chase target.
			const tz::vec2 target_pos = chase_target.value();
			tz::vec2 dist_to_target = target_pos - quad.position;
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
			if(actor().flags.has<FlagID::WanderIfIdle>())
			{
				const auto& flag = actor().flags.get<FlagID::WanderIfIdle>()->data();
				std::uniform_real_distribution<float> dist{0.0f, 1.0f};
				if(dist(this->rng) < flag.wander_chance)
				{
					std::uniform_real_distribution<float> dist2(flag.max_wander_range * -0.5f, flag.max_wander_range * 0.5f);
					// Set an action to wander to a nearby location.
					tz::vec2 target_loc = quad.position;
					target_loc += tz::vec2{dist2(this->rng), dist2(this->rng)};
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
			tz::vec2 position_change{0.0f, 0.0f};
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
		return this->garbage_collect(id) ? ActorPostUpdateResult::ActorDeleted : ActorPostUpdateResult::Typical;
	}

	std::vector<std::size_t> Scene::get_living_players() const
	{
		TZ_PROFZONE("Scene - Query Living Players", 0xFF00AA00);
		std::vector<std::size_t> ret;
		for(std::size_t i = 0; i < this->size(); i++)
		{
			
			if(this->get_actor(i).flags.has<FlagID::Player>() && !this->get_actor(i).dead())
			{
				ret.push_back(i);
			}
		}
		return ret;
	}

	std::optional<std::size_t> Scene::find_first_player() const
	{
		TZ_PROFZONE("Scene - Find First Player", 0xFF00AA00);
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
		tz::vec2 scale = quad.scale;
		// Bounding box should be affected by custom reach.
		const Actor& actor = this->get_actor(actor_id);
		if(actor.flags.has<FlagID::CustomReach>())
		{
			const auto& flag = actor.flags.get<FlagID::CustomReach>()->data();
			scale *= flag.reach;
		}
		tz::vec2 min{-0.5f, -0.5f};
		tz::vec2 max{0.5f, 0.5f};
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
		TZ_PROFZONE("Scene - Collision Query", 0xFF00AA00);
		QuadtreeNode a_node{.actor_id = actor_a};
		QuadtreeNode b_node{.actor_id = actor_b};
		return std::find_if(this->intersections.begin(), this->intersections.end(), [&a_node, b_node](const auto& pair)
		{
			return (pair.first == a_node && pair.second == b_node)
			    || (pair.first == b_node && pair.second == a_node);
		}) != this->intersections.end();
	}

	std::size_t Scene::get_actor_from_uuid(std::size_t uuid) const
	{
		auto iter = std::find_if(this->actors.begin(), this->actors.end(), [uuid](const Actor& a){return a.uuid == uuid;});
		return iter != this->actors.end() ? std::distance(this->actors.begin(), iter) : Actor::NullID;
	}

	bool Scene::is_in_bounds(std::size_t actor_id) const
	{
		TZ_PROFZONE("Scene - Bounds Check", 0xFF00AA00);
		tz::vec2 pos = this->qrenderer.elements()[actor_id].position;
		auto bounds = this->get_world_boundaries();
		return
			bounds.first[0] <= pos[0] && pos[0] <= bounds.second[0]
		     && bounds.first[1] <= pos[1] && pos[1] <= bounds.second[1];
	}

	bool Scene::is_in_level(std::size_t actor_id) const
	{
		TZ_PROFZONE("Scene - Level Bounds Check", 0xFF00AA00);
		tz::vec2 pos = this->qrenderer.elements()[actor_id].position;
		tz::vec2 level_min = this->level_boundaries * -1.0f;
		return
			level_min[0] <= pos[0] && pos[0] <= this->level_boundaries[0]
		     && level_min[1] <= pos[1] && pos[1] <= this->level_boundaries[1];
	}

	std::pair<tz::vec2, tz::vec2> Scene::get_world_boundaries() const
	{
		const float width_mod = this->qrenderer.get_width_multiplier();
		const tz::vec2 camera_pos = this->qrenderer.camera_position();
		return
		{
			tz::vec2{-width_mod, -1.0f} + camera_pos, tz::vec2{width_mod, 1.0f} + camera_pos
		};
	}

	void Scene::update_camera()
	{
		TZ_PROFZONE("Scene - Camera Update", 0xFF00AA00);

		// Firstly check zoom.
		int ybefore = this->mouse_scroll_data;	
		int ynow = tz::window().get_mouse_state().wheel_position;
		const float zoom = this->qrenderer.get_camera_zoom();
		this->mouse_scroll_data = ynow;
		if(ynow > ybefore)
		{
			// scrolled up.
			this->qrenderer.set_camera_zoom(zoom - 0.1f);
		}
		else if(ynow < ybefore)
		{
			// scrolled down.
			this->qrenderer.set_camera_zoom(zoom + 0.1f);
		}

		// Then, move camera to cover all players.

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
		tz::vec2 min{fmax, fmax}, max{fmin, fmin}, avg{0.0f, 0.0f};
		for(std::size_t actor_id : actors_to_view)
		{
			tz::vec2 pos = this->qrenderer.elements()[actor_id].position;
			min[0] = std::min(min[0], pos[0]);
			min[1] = std::min(min[1], pos[1]);
			max[0] = std::max(max[0], pos[0]);
			max[1] = std::max(max[1], pos[1]);
			avg[0] += pos[0];
			avg[1] += pos[1];
		}
		avg /= static_cast<float>(actors_to_view.size());
		// We want to view 'avg'. If it's far enough from the camera by some constant we will start to move towards it.
		const tz::vec2 camera_displacement = avg - this->qrenderer.camera_position();
		if(camera_displacement.length() > (0.5f * this->qrenderer.get_camera_zoom()))
		{
			
#if 1
			this->qrenderer.camera_position() += camera_displacement * 0.02f * camera_displacement.length() / this->qrenderer.get_camera_zoom();
#else
			this->qrenderer.camera_position() = avg;
#endif
		}
	}

	void Scene::update_status_events(std::size_t id)
	{
		TZ_PROFZONE("Scene - Status Events Update", 0xFF00AA00);
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
		TZ_PROFZONE("Scene - Quadtree Update", 0xFF00AA00);
		this->quadtree.add({.actor_id = actor_id, .bounding_box = this->get_bounding_box(actor_id)});
	}

	bool Scene::garbage_collect(std::size_t id)
	{
		TZ_PROFZONE("Scene - Garbage Collect", 0xFF00AA00);
		// Erase means to swap with the last and then pop it back
		const bool dead = this->get_actor(id).dead();
		if(dead)
		{
			auto iter = this->despawn_timer.find(id);
			if(iter != this->despawn_timer.end())
			{
				// It has an entry, see if its timed out.
				if(iter->second.done() && !this->get_actor(id).flags.has<FlagID::DoNotGarbageCollect>())
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
				if(this->get_actor(id).flags.has<FlagID::CustomGarbageCollectPeriod>())
				{
					tz::duration d{this->get_actor(id).flags.get<FlagID::CustomGarbageCollectPeriod>()->data().delay_millis};
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
		TZ_PROFZONE("Scene - Collision Resolution", 0xFF00AA00);
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
		TZ_PROFZONE("Scene - Single Collision Resolution", 0xFF00AA00);
		Actor& actor = this->get_actor(a_id);
		QuadRenderer::ElementData& quad = this->qrenderer.elements()[a_id];
		Actor& other = this->get_actor(b_id);
		QuadRenderer::ElementData& other_quad = this->qrenderer.elements()[b_id];

		bool is_hazardous = actor.flags.has<FlagID::HazardousIf>();
		if(is_hazardous)
		{
			const auto& flag = actor.flags.get<FlagID::HazardousIf>()->data();
			is_hazardous = flag.predicate(actor, other);
		}

		const bool wants_to_hurt = (is_hazardous) && actor.is_enemy_of(other) && !other.flags.has<FlagID::Unhittable>() && !actor.dead() && !other.dead();
		bool is_collider_candidate = actor.flags.get<FlagID::Collide>();
		if(is_collider_candidate)
		{
			auto& flag = actor.flags.get<FlagID::Collide>()->data();
			bool whitelisted = false;
			if(flag.collision_filter.contains(other.type) || (flag.filter_predicate != nullptr && flag.filter_predicate(other)))
			{
				whitelisted = true;
			}
			if(!flag.collision_filter.empty() || flag.filter_predicate != nullptr)
			{
				is_collider_candidate &= whitelisted;
			}
			is_collider_candidate &= !flag.collision_blacklist.contains(other.type);
			if(flag.blacklist_predicate != nullptr)
			{
				is_collider_candidate &= !flag.blacklist_predicate(other);
			}
		}
		const bool blocks_colliders = actor.flags.has<FlagID::Collide>() &&
			!actor.dead() && !other.dead() &&
			is_collider_candidate;
		bool wants_touch_other = false;
		if(actor.flags.has<FlagID::ActionOnActorTouch>())
		{
			auto flag = actor.flags.get<FlagID::ActionOnActorTouch>();
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
				// Get boxes.
				Box b_box = this->get_bounding_box(a_id);
				Box a_box = this->get_bounding_box(b_id);
				float overlap_x = calculate_overlap(b_box.get_left(), b_box.get_right(), a_box.get_left(), a_box.get_right());
				float overlap_y = calculate_overlap(b_box.get_bottom(), b_box.get_top(), a_box.get_bottom(), a_box.get_top());
				float correction = std::min(overlap_x, overlap_y) * actor.density;
				if(overlap_x < overlap_y)
				{
					// Solve based on x
					if(b_box.get_centre()[0] < a_box.get_centre()[0])
					{
						other_quad.position[0] += correction;
					}
					else
					{
						other_quad.position[0] -= correction;
					}
				}
				else
				{
					// Solve based on y
					if(b_box.get_centre()[1] < a_box.get_centre()[1])
					{
						other_quad.position[1] += correction;
					}
					else
					{
						other_quad.position[1] -= correction;
					}
				}

			}
			if(wants_touch_other)
			{
				// Actor is touching the subject, and it wants to.
				// Carry out its actions.
				auto& on_touch_flag = actor.flags.get<FlagID::ActionOnActorTouch>()->data();
				on_touch_flag.actions.copy_components(actor.entity);
				on_touch_flag.touchee_actions.copy_components(other.entity);
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
		TZ_PROFZONE("Scene - On Actor Hit", 0xFF8B4513);
		if(e.attacker.flags.has<FlagID::ActionOnHit>())
		{
			auto& flag = e.attacker.flags.get<FlagID::ActionOnHit>()->data();
			flag.actions.copy_components(e.attacker.entity);
			flag.hittee_actions.copy_components(e.attackee.entity);
		}
		if(e.attacker.flags.has<FlagID::SelfRecoil>())
		{
			this->do_actor_hit(this->world_actor, e.attacker);
		}
	}

	void Scene::on_actor_struck(ActorStruckEvent e)
	{
		TZ_PROFZONE("Scene - On Actor Struck", 0xFF8B4513);
		//tz::report("%s struck by %s", e.attackee.name, e.attacker.name);
		if(e.attackee.flags.has<FlagID::ActionOnStruck>())
		{
			auto& flag = e.attackee.flags.get<FlagID::ActionOnStruck>()->data();
			if(flag.internal_cooldown > 0.0f)
			{
				flag.internal_cooldown -= (tz::system_time() - e.attackee.last_update).millis<float>();
			}
			else
			{
				flag.internal_cooldown = flag.icd;
				flag.actions.copy_components(e.attackee.entity);
				flag.striker_actions.copy_components(e.attacker.entity);
			}
		}
	}

	void Scene::on_actor_kill(ActorKillEvent e)
	{
		TZ_PROFZONE("Scene - On Actor Kill", 0xFF8B4513);
		//tz::report("%s killed %s", e.killer.name, e.killee.name);
	}

	void Scene::on_actor_death(ActorDeathEvent e)
	{
		TZ_PROFZONE("Scene - On Actor Death", 0xFF8B4513);
		// If something dies, it's not moving anymore.
		e.killee.motion = {};
		//tz::report("%s killed by %s", e.killee.name, e.killer.name);
		if(e.killee.flags.has<FlagID::ActionOnDeath>())
		{
			auto& flag = e.killee.flags.get<FlagID::ActionOnDeath>()->data();
			flag.actions.copy_components(e.killee.entity);
		}
		if(e.killee.flags.has<FlagID::RespawnOnDeath>())
		{
			e.killee.entity.add<ActionID::Respawn>();
		}
	}
}

