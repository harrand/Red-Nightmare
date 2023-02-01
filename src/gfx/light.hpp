#ifndef REDNIGHTMARE_SRC_LIGHT_HPP
#define REDNIGHTMARE_SRC_LIGHT_HPP
#include "tz/core/data/vector.hpp"

namespace game
{
	struct PointLight
	{
		tz::vec2 position{0.0f, 0.0f};
		std::uint32_t emits_pixels = true;
		std::uint32_t emits_light = true;
		tz::vec3 colour{1.0f, 1.0f, 1.0f};
		float power = 0.0f;
	};

	struct EmissiveActorInfo
	{
		PointLight light;
		bool emit_pixels = true;
		bool emit_light = true;
		tz::vec2 offset = tz::vec2::zero();
		// How much should the power of the light change sinusoidally over time?
		float power_variance = 0.0f;
		// How quickly should the variance occur (1.0f means sin(time in seconds))
		float variance_rate = 1.0f;
		// What is the minimum % of original power that should be possible during variance?
		float min_variance_pct = 0.3f;
		// What is the maximum % of original power that should be possible during variance?
		float max_variance_pct = 1.5f;
		// If true, the power of the light is proportional to the % health of the actor. If false, no change.
		bool power_scale_with_health_pct = false;
		// Impl detail. Don't touch this.
		float time = 0.0f;
	};
}

#endif // REDNIGHTMARE_SRC_LIGHT_HPP
