#ifndef REDNIGHTMARE_SCENE_HPP
#define REDNIGHTMARE_SCENE_HPP
#include "actor.hpp"
#include "animation.hpp"
#include "quad_renderer.hpp"

#include <random>

namespace game
{
	class Scene
	{
	public:
		Scene() = default;
		void render();
		void update();
		void dbgui();
		std::size_t size() const;
		bool empty() const;

		void add(ActorType type);
		void pop();
		void clear();
	private:
		void actor_post_update(std::size_t id);
		std::optional<std::size_t> find_first_player() const;
		bool actor_collision_query(std::size_t actor_a, std::size_t actor_b) const;

		QuadRenderer qrenderer;
		std::vector<Actor> actors;
		std::default_random_engine rng;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
