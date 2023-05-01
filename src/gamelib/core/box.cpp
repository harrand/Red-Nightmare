#include "gamelib/core/box.hpp"

namespace rnlib
{
	box::box(tz::vec2 min, tz::vec2 max):
	bottom_left(min),
	length(max - min){}

	float box::get_left() const
	{
		return this->bottom_left[0];
	}

	float box::get_right() const
	{
		return this->get_left() + this->length[0];
	}

	float box::get_top() const
	{
		return this->get_bottom() + this->length[1];
	}

	float box::get_bottom() const
	{
		return this->bottom_left[1];
	}

	tz::vec2 box::get_centre() const
	{
		return {this->get_left() + (length[0] * 0.5f), this->get_bottom() + (length[1] * 0.5f)};
	}

	tz::vec2 box::get_dimensions() const
	{
		return this->length;
	}

	bool box::contains(const box& box) const
	{
		return this->get_left() <= box.get_left() &&
		       this->get_right() >= box.get_right() &&
		       this->get_bottom() <= box.get_bottom() &&
		       this->get_top() >= box.get_top();
	}

	bool box::intersects(const box& box) const
	{
		return !(this->get_left() >= box.get_right() ||
			this->get_right() <= box.get_left() ||
			this->get_bottom() >= box.get_top() ||
			this->get_top() <= box.get_bottom());
	}
}
