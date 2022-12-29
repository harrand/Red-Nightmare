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
					.levels =
					{
						game::load_level(LevelID::DevLevel1),
						game::load_level(LevelID::DevLevel0)
					}
				};
			break;
		}
		hdk::error("Unimplemented story zone");
		return {};
	}
}
