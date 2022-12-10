#ifndef REDNIGHTMARE_BOX_HPP
#define REDNIGHTMARE_BOX_HPP
#include "hdk/data/vector.hpp"

namespace game
{
	class Box
	{
		public:
			Box(hdk::vec2 min, hdk::vec2 max);
			float get_left() const;
			float get_right() const;
			float get_top() const;
			float get_bottom() const;
			hdk::vec2 get_centre() const;
			hdk::vec2 get_dimensions() const;

			bool contains(const Box& box) const;
			bool intersects(const Box& box) const;

			bool operator==(const Box& rhs) const = default;
		private:
			hdk::vec2 bottom_left;
			hdk::vec2 length;
	};
}

#endif // REDNIGHTMARE_BOX_HPP
