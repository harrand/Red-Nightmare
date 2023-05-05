#ifndef RNLIB_GAMEPLAY_ACTOR_SPELL_HPP
#define RNLIB_GAMEPLAY_ACTOR_SPELL_HPP
#include "tz/core/data/vector.hpp"
#include <functional>

namespace rnlib
{
	enum class spell_id
	{
		heal
	};

	enum class cast_direction
	{
		// casting something at a specific thing/place
		directed,
		// casting in no general direction (e.g a self heal)
		omni
	};


	struct spell_cast_info
	{
		tz::vec3 colour = tz::vec3::filled(1.0f);
		cast_direction direction = cast_direction::omni;	
		float cast_time_seconds = 0.0f;

		bool is_instant() const{return this->cast_time_seconds == 0.0f;}
	};

	class actor;
	class actor_system;
	using spell_on_cast_t = std::function<void(actor&, actor_system&)>;

	struct spell
	{
		spell_id id;
		spell_cast_info cast = {};
		spell_on_cast_t function = [this](actor&, actor_system&)
		{
			#if TZ_DEBUG
				tz::report("Spell cast %d detected", static_cast<int>(this->id));
			#endif
		};
		const char* name = "";
	};

	spell create_spell(spell_id id);
}

#endif // RNLIB_GAMEPLAY_ACTOR_SPELL_HPP
