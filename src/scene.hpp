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
#include <span>

namespace game
{
	struct SceneData
	{
		const std::size_t this_id;
		std::span<Actor> actors;
		std::span<QuadRenderer::ElementData> quads;

		std::optional<hdk::vec2> chase_target = std::nullopt;

		const hdk::vec2 mouse_position;

		Actor& get_actor(std::size_t aid){return this->actors[aid];}
		QuadRenderer::ElementData& get_quad(std::size_t aid){return this->quads[aid];}

		Actor& actor(){return get_actor(this_id);}
		QuadRenderer::ElementData& quad(){return get_quad(this_id);}
	};

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
		const Actor& get_actor(std::size_t id) const;
		Actor& get_actor(std::size_t id);

		void impl_load_level(const Level& level);
		hdk::vec2 get_mouse_position() const;
		void erase(std::size_t id);
		bool actor_post_update(std::size_t id);
		std::vector<std::size_t> get_living_players() const;
		std::optional<std::size_t> find_first_player() const;
		Box get_bounding_box(std::size_t actor_id) const;
		bool actor_collision_query(std::size_t actor_a, std::size_t actor_b) const;
		bool is_in_bounds(std::size_t actor_id) const;
		bool is_in_level(std::size_t actor_id) const;
		std::pair<hdk::vec2, hdk::vec2> get_world_boundaries() const;
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

		Quadtree<QuadtreeNode> quadtree{Box{hdk::vec2{-100.0f, -100.0f}, hdk::vec2{100.0f, 100.0f}}};
		Quadtree<QuadtreeNode>::IntersectionState intersections = {};
		QuadRenderer qrenderer;
		std::vector<Actor> actors;
		Actor world_actor = game::create_actor(ActorType::World);
		std::default_random_engine rng;
		std::unordered_map<std::size_t, tz::Delay> despawn_timer;
		ActorEventHandler events;
		hdk::vec2 level_boundaries = hdk::vec2::zero();
		mutable std::size_t debug_collision_query_count = 0;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
