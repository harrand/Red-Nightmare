#ifndef RNLIB_CORE_QUADTREE_HPP
#define RNLIB_CORE_QUADTREE_HPP
#include "tz/core/data/vector.hpp"
#include <cstddef>

namespace rnlib
{
	class quadtree
	{
		// represents a thing that's somewhere within the quadtree. e.g an actor.
		struct element
		{
			std::size_t usrid; // id of the element.
			tz::vec2 min;
			tz::vec2 max;
		};
	};
}

#endif // RNLIB_CORE_QUADTREE_HPP
