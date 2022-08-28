#ifndef REDNIGHTMARE_SCENE_HPP
#define REDNIGHTMARE_SCENE_HPP
#include "animation.hpp"
#include "quad_renderer.hpp"

namespace game
{
	class Scene
	{
	public:
		Scene() = default;
		void render();
		void dbgui();
		std::size_t size() const;
		bool empty() const;

		void temp_add();
		void pop();
		void clear();
	private:
		QuadRenderer qrenderer;
		std::vector<Animation> animations;
	};
}

#endif // REDNIGHTMARE_SCENE_HPP
