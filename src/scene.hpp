#ifndef REDNIGHTMARE_SCENE_HPP
#define REDNIGHTMARE_SCENE_HPP
#include "actor.hpp"
#include "animation.hpp"
#include "quad_renderer.hpp"

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

		void temp_add();
		void add(ActorType type);
		void pop();
		void clear();
	private:
		void actor_post_update(std::size_t id);

		QuadRenderer qrenderer;
		std::vector<Actor> actors;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
