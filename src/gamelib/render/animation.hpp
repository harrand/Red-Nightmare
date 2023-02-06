#ifndef RNLIB_RENDER_ANIMATION_HPP
#define RNLIB_RENDER_ANIMATION_HPP
#include <vector>
#include <cstdint>

namespace rnlib
{
	struct animation_data
	{
		std::vector<std::uint32_t> frame_textures;
		unsigned int fps;
		bool loop;
	};

	class animation
	{
	public:
		animation(animation_data data);
		std::uint32_t get_image() const;
		void update(float dt);
		void dbgui();
	private:
		animation_data data;
		float time_multiplier = 1.0f;
		float elapsed = 0.0f;
	};
}

#endif // RNLIB_RENDER_ANIMATION_HPP
