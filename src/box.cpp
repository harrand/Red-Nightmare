#include "box.hpp"

namespace game
{
	Box::Box(hdk::vec2 min, hdk::vec2 max):
	bottom_left(min),
	length(max - min){}

	float Box::get_left() const
	{
		return this->bottom_left[0];
	}

	float Box::get_right() const
	{
		return this->get_left() + this->length[0];
	}

	float Box::get_top() const
	{
		return this->get_bottom() + this->length[1];
	}

	float Box::get_bottom() const
	{
		return this->bottom_left[1];
	}

	hdk::vec2 Box::get_centre() const
	{
		return {this->get_left() + (length[0] * 0.5f), this->get_bottom() + (length[1] * 0.5f)};
	}

	hdk::vec2 Box::get_dimensions() const
	{
		return this->length;
	}

	bool Box::contains(const Box& box) const
	{
		return this->get_left() <= box.get_left() &&
		       this->get_right() >= box.get_right() &&
		       this->get_bottom() <= box.get_bottom() &&
		       this->get_top() >= box.get_top();
	}

	bool Box::intersects(const Box& box) const
	{
		return !(this->get_left() >= box.get_right() ||
			this->get_right() <= box.get_left() ||
			this->get_bottom() >= box.get_top() ||
			this->get_top() <= box.get_bottom());
	}
}
