#include "zone.hpp"

namespace game
{
	Zone get_story_zone(StoryZone zone)
	{
		switch(zone)
		{
			case StoryZone::BlanchfieldGraveyard:
				return
				{
					.initial_spawn = hdk::vec2{15.88f, 7.38f},
					.levels =
					{
						game::load_level(LevelID::DevLevel1),
						game::load_level(LevelID::DevLevel0)
					},
				};
			break;
		}
		hdk::error("Unimplemented story zone");
		return {};
	}
}
