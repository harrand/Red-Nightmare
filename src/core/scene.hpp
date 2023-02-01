#ifndef REDNIGHTMARE_SCENE_HPP
#define REDNIGHTMARE_SCENE_HPP
#include "core/event.hpp"
#include "core/actor.hpp"
#include "gfx/animation.hpp"
#include "gfx/quad_renderer.hpp"
#include "core/level.hpp"
#include "core/zone.hpp"
#include "core/quadtree.hpp"
#include "gfx/effect.hpp"
#include "tz/core/time.hpp"
#include "tz/dbgui/dbgui.hpp"

#include <random>
#include <span>

namespace game
{
	struct SceneData
	{
		const std::size_t this_id;

		const tz::vec2 mouse_position;
		std::optional<tz::vec2>& chase_target;
		float& touch_distance;
		std::default_random_engine& rng;
		std::function<std::vector<std::size_t>()> get_living_players;
		std::function<std::pair<tz::vec2, tz::vec2>()> get_world_boundaries;
		std::function<std::size_t(ActorType)> spawn_actor;
		std::function<Actor&(std::size_t)> get_actor;
		std::function<QuadRenderer::ElementData&(std::size_t)> get_quad;
		std::function<std::size_t(std::size_t)> get_actor_from_uuid;
		std::function<void()> next_level;
		std::function<void()> previous_level;

		Actor& actor(){return this->get_actor(this_id);}
		QuadRenderer::ElementData& quad(){return this->get_quad(this_id);}
	};

	enum class ActorPostUpdateResult
	{
		Typical,
		ActorDeleted,
		LevelDeleted
	};

	class Scene
	{
	public:
		Scene();
		Scene(const Scene& copy) = delete;
		Scene(Scene&& move) = delete;
		~Scene();
		Scene& operator=(const Scene& rhs) = delete;
		Scene& operator=(Scene&& rhs) = delete;

		void render();
		void update();
		void dbgui_current_scene();
		void dbgui_story_levels();
		void dbgui_procedural_level();
		std::size_t size() const;
		bool empty() const;

		void add(ActorType type);
		void pop();
		void clear();
		void load_zone(StoryZone zone);
		void load_level(LevelID level_id);
	private:
		const Actor& get_actor(std::size_t id) const;
		Actor& get_actor(std::size_t id);

		void impl_load_zone(Zone zone);
		void impl_load_level(const Level& level);
		void impl_next_level();
		void impl_prev_level();
		tz::vec2 get_mouse_position() const;
		void erase(std::size_t id);
		ActorPostUpdateResult actor_post_update(std::size_t id);
		std::vector<std::size_t> get_living_players() const;
		std::optional<std::size_t> find_first_player() const;
		Box get_bounding_box(std::size_t actor_id) const;
		bool actor_collision_query(std::size_t actor_a, std::size_t actor_b) const;
		std::size_t get_actor_from_uuid(std::size_t uuid) const;

		bool is_in_bounds(std::size_t actor_id) const;
		bool is_in_level(std::size_t actor_id) const;
		std::pair<tz::vec2, tz::vec2> get_world_boundaries() const;
		void update_camera();
		void update_status_events(std::size_t id);
		void update_quadtree(std::size_t actor_id);
		bool garbage_collect(std::size_t id);
		void collision_resolution();
		void resolve_collision(std::size_t a_id, std::size_t b_id);
		void do_actor_hit(std::size_t attacker, std::size_t attackee);
		void do_actor_hit(Actor& attacker, Actor& attackee);
		
		void on_actor_hit(ActorHitEvent e);
		void on_actor_struck(ActorStruckEvent e);
		void on_actor_kill(ActorKillEvent e);
		void on_actor_death(ActorDeathEvent e);

		struct QuadtreeNode
		{
			std::size_t actor_id;
			Box bounding_box = {{}, {0.0f, 0.0f}};

			const Box& get_box() const{return this->bounding_box;}
			bool operator==(const QuadtreeNode& rhs) const{return this->actor_id == rhs.actor_id;}
		};

		Quadtree<QuadtreeNode> quadtree{Box{tz::vec2{-100.0f, -100.0f}, tz::vec2{100.0f, 100.0f}}};
		Quadtree<QuadtreeNode>::IntersectionState intersections = {};
		QuadRenderer qrenderer;
		std::vector<Actor> actors;
		Actor world_actor = game::create_actor(ActorType::World);
		std::default_random_engine rng;
		int mouse_scroll_data = 0;
		std::unordered_map<std::size_t, tz::delay> despawn_timer;
		ActorEventHandler events;
		tz::vec2 level_boundaries = tz::vec2::zero();
		mutable std::size_t debug_collision_query_count = 0;
		std::size_t impl_light_actor_count = 0;
		Zone zone = {};
		tz::callback_handle game_bar_dbgui_callback = tz::nullhand;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
