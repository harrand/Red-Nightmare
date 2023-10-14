#include "gamelib/physics/aabb.hpp"

namespace game::physics
{
	aabb::aabb(tz::vec2 min, tz::vec2 max):
	bottom_left(min),
	length(max - min){}

	float aabb::get_left() const
	{
		return this->bottom_left[0];
	}

	float aabb::get_right() const
	{
		return this->get_left() + this->length[0];
	}

	float aabb::get_top() const
	{
		return this->get_bottom() + this->length[1];
	}

	float aabb::get_bottom() const
	{
		return this->bottom_left[1];
	}

	tz::vec2 aabb::get_centre() const
	{
		return {this->get_left() + (length[0] * 0.5f), this->get_bottom() + (length[1] * 0.5f)};
	}

	tz::vec2 aabb::get_dimensions() const
	{
		return this->length;
	}

	bool aabb::contains(const aabb& rhs) const
	{
		return this->get_left() <= rhs.get_left() &&
		       this->get_right() >= rhs.get_right() &&
		       this->get_bottom() <= rhs.get_bottom() &&
		       this->get_top() >= rhs.get_top();
	}

	bool aabb::intersects(const aabb& rhs) const
	{
		return !(this->get_left() >= rhs.get_right() ||
			this->get_right() <= rhs.get_left() ||
			this->get_bottom() >= rhs.get_top() ||
			this->get_top() <= rhs.get_bottom());
	}
}