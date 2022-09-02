#ifndef REDNIGHTMARE_STATS_HPP
#define REDNIGHTMARE_STATS_HPP

namespace game
{
	constexpr float default_base_movement = 0.0f;
	constexpr float default_base_damage = 0.0083f;
	constexpr float default_max_health = 10;

	struct Stats
	{
		float base_movement_speed = default_base_movement;
		float base_damage = default_base_damage;
		float max_health = default_max_health;
		float current_health = default_max_health;
		float defence = 0.0f;
	};

	struct StatBuff
	{
		float multiply_speed_boost = 1.0f;
		float add_speed_boost = 0.0f;
		float multiply_damage = 1.0f;
		float add_damage = 0.0f;
		float multiply_defense = 1.0f;
		float add_defense = 0.0f;
	};
}

#endif // REDNIGHTMARE_STATS_HPP
