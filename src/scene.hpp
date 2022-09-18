#ifndef REDNIGHTMARE_SCENE_HPP
#define REDNIGHTMARE_SCENE_HPP
#include "event.hpp"
#include "actor.hpp"
#include "animation.hpp"
#include "quad_renderer.hpp"
#include "level.hpp"
#include "quadtree.hpp"
#include "tz/core/time.hpp"

#include <random>

namespace game
{
	class Scene
	{
	public:
		Scene();
		void render();
		void update();
		void dbgui();
		std::size_t size() const;
		bool empty() const;

		void add(ActorType type);
		void pop();
		void clear();
		void load_level(LevelID level_id);
	private:
		void impl_load_level(const Level& level);
		tz::Vec2 get_mouse_position() const;
		void erase(std::size_t id);
		void actor_post_update(std::size_t id);
		std::vector<std::size_t> get_living_players() const;
		std::optional<std::size_t> find_first_player() const;
		Box get_bounding_box(std::size_t actor_id) const;
		bool actor_collision_query(std::size_t actor_a, std::size_t actor_b) const;
		bool is_in_bounds(std::size_t actor_id) const;
		std::pair<tz::Vec2, tz::Vec2> get_world_boundaries() const;
		void update_camera();
		void update_status_events(std::size_t id);
		void update_quadtree(std::size_t actor_id);
		void garbage_collect(std::size_t id);
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

		Quadtree<QuadtreeNode> quadtree{Box{tz::Vec2{-100.0f, -100.0f}, tz::Vec2{100.0f, 100.0f}}};
		Quadtree<QuadtreeNode>::IntersectionState intersections = {};
		QuadRenderer qrenderer;
		std::vector<Actor> actors;
		Actor world_actor = game::create_actor(ActorType::World);
		std::default_random_engine rng;
		std::unordered_map<std::size_t, tz::Delay> despawn_timer;
		ActorEventHandler events;
		mutable std::size_t debug_collision_query_count = 0;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
