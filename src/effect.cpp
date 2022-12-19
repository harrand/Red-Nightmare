#include "effect.hpp"
#include "hdk/debug.hpp"
#include <memory>

namespace game
{
	std::unique_ptr<EffectManager> emgr = nullptr;

	namespace effects_impl
	{
		void initialise()
		{
			hdk::assert(emgr == nullptr, "Initialise: Double initialise detected");
			emgr = std::make_unique<EffectManager>();
		}

		void terminate()
		{
			hdk::assert(emgr != nullptr, "Terminate: Double terminate, or no initialise detected");
			emgr = nullptr;
		}
	}

	EffectManager& effects()
	{
		hdk::assert(emgr != nullptr, "game::effects(): No initialise detected");
		return *emgr;
	}
}
