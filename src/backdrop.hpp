#ifndef REDNIGHTMARE_SRC_BACKDROP_HPP
#define REDNIGHTMARE_SRC_BACKDROP_HPP
#include "images.hpp"

namespace game
{
	struct Backdrop
	{
		TextureID foreground = TextureID::Invisible;
		TextureID background = TextureID::Dungeon_Floor_Generic_Backdrop;
	};

}

#endif // REDNIGHTMARE_SRC_BACKDROP_HPP
