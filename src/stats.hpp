#ifndef REDNIGHTMARE_STATS_HPP
#define REDNIGHTMARE_STATS_HPP
#include <vector>
#include <limits>
#include <span>

namespace game
{
	constexpr float default_base_movement = 0.0005f;
	constexpr float default_base_damage = 0.0083f;
	constexpr float default_max_health = 10;

	struct Stats
	{
		float movement_speed = default_base_movement;
		float damage = default_base_damage;
		float max_health = default_max_health;
		float current_health = default_max_health;
		float defense = 0.0f;

		void dbgui() const;
	};

	struct StatBuff
	{
		float multiply_speed_boost = 1.0f;
		float add_speed_boost = 0.0f;
		float multiply_damage = 1.0f;
		float add_damage = 0.0f;
		float multiply_defense = 1.0f;
		float add_defense = 0.0f;
		float multiply_health = 1.0f;
		float add_health = 0.0f;

		bool operator==(const StatBuff& rhs) const;

		/// Default buff duration is 2 minutes.
		float time_remaining_millis = std::numeric_limits<float>::max();
		unsigned int stacks = 1;
		unsigned int max_stacks = 1;
	};

	enum class BuffID
	{
		/// 1000% damage forever.
		Berserk,
		/// 70% more movement speed for 5 seconds.
		Sprint,
		/// 70% less movement speed for 5 seconds.
		Chill,
		/// 30% more movement speed for 0.5 seconds.
		RoadSpeed,
	};

	StatBuff get_buff(BuffID buff);

	class StatBuffs
	{
	public:
		StatBuffs() = default;
		void update();
		void add(BuffID buff);
		bool contains(BuffID buff) const;
		void erase(std::size_t id);
		std::span<const StatBuff> elements() const;
		std::span<StatBuff> elements();
	private:
		std::vector<StatBuff> buffs;
	};
}

#endif // REDNIGHTMARE_STATS_HPP
