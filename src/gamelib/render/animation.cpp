#include "gamelib/render/animation.hpp"
#include <algorithm>

namespace rnlib
{
	animation::animation(animation_data data):
	data(data){}

	std::uint32_t animation::get_image() const
	{
		const auto frame_count = static_cast<unsigned int>(this->data.frame_textures.size());
		const float frame_period = static_cast<float>(frame_count) / this->data.fps;

		float true_elapsed = this->elapsed * this->time_multiplier;
		unsigned int frame_number = true_elapsed / frame_period;
		if(!this->data.loop)
		{
			frame_number = std::clamp(frame_number, 0u, frame_count - 1);
		}
		else
		{
			frame_number = frame_number % frame_count;
		}
		return this->data.frame_textures[frame_number];
	}

	void animation::update(float dt)
	{
		// assume dt is in millis.
		this->elapsed += (dt / 1000.0f);
	}
}
