#ifndef RNLIB_GAMEPLAY_ACTOR_COMBAT_LOG_HPP
#define RNLIB_GAMEPLAY_ACTOR_COMBAT_LOG_HPP
#include <cstddef>
#include <span>
#include <vector>

namespace rnlib
{
	enum class combat_text_type;
	enum class spell_id;

	struct combat_event
	{
		spell_id spell;
		std::size_t caster_uuid;
		std::size_t target_uuid;
		combat_text_type type;
		std::size_t value;

		static combat_event null();
	};

	using combat_events = std::vector<combat_event>;

	class combat_log
	{
	public:
		combat_log() = default;
		std::span<const combat_event> container() const;
		void add(combat_event evt);
		std::size_t size() const;
	private:
		std::vector<combat_event> log;
	};
}

#endif // RNLIB_GAMEPLAY_ACTOR_COMBAT_LOG_HPP
