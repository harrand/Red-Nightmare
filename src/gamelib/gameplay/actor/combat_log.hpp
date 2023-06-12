#ifndef RNLIB_GAMEPLAY_ACTOR_COMBAT_LOG_HPP
#define RNLIB_GAMEPLAY_ACTOR_COMBAT_LOG_HPP
#include "tz/core/data/enum_field.hpp"
#include <cstddef>
#include <span>
#include <vector>
#include <optional>

namespace rnlib
{
	enum class combat_text_type;
	enum class spell_id;

	enum class combat_damage_type
	{
		untyped,
		physical,
		fire,
		frost,
		earth,
		air,
		shadow,
		nihimancy,
		divine
	};

	using combat_damage_types = tz::enum_field<combat_damage_type>;


	struct combat_event
	{
		spell_id spell;
		std::size_t caster_uuid;
		std::size_t target_uuid;
		combat_text_type type;
		std::size_t value;
		combat_damage_types damage_type = {};
		std::optional<std::size_t> over = std::nullopt;

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
