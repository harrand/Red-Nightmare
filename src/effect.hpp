#ifndef REDNIGHTMARE_EFFECT_HPP
#define REDNIGHTMARE_EFFECT_HPP
#include "tz/gl/resource.hpp"

namespace game
{
	enum class EffectID
	{
		None,
		Rain,
		Count
	};

	class EffectManager
	{
	public:
		EffectManager() = default;
		EffectManager(const EffectManager& copy) = delete;
		EffectManager(EffectManager&& move) = delete;
	private:
	};

	namespace effects_impl
	{
		void initialise();
		void terminate();
	}

	EffectManager& effects();
}

#endif // REDNIGHTMARE_EFFECT_HPP
