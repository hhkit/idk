#include "stdafx.h"
#include "aabb.h"
#include <ds/zip.h>

namespace idk
{
	vec3 aabb::center() const
	{
		return (min + max) / 2;
	}
	
	vec3 aabb::size() const
	{
		return max - min;
	}

	vec3 aabb::extents() const
	{
		return size() / 2;
	}

	const aabb& aabb::bounds() const
	{
		return *this;
	}
	
	aabb& aabb::translate(const vec3& trans)
	{
		min += trans;
		max += trans;
		return *this;
	}

	aabb& aabb::center_at(const vec3& pos)
	{
		return translate(pos - center());
	}

	aabb& aabb::surround(const vec3& point)
	{
		for (auto [elem, emin, emax] : zip(point, min, max))
		{
			emin = std::min(elem, emin);
			emax = std::max(elem, emax);
		}
		return *this;
	}

	aabb& aabb::surround(const aabb& rhs)
	{
		for (auto [curr, update] : zip(min, rhs.min))
			curr = std::min(curr, update);

		for (auto [curr, update] : zip(max, rhs.max))
			curr = std::max(curr, update);

		return *this;
	}
	
	aabb& aabb::grow(const vec3& rhs)
	{
		for (auto [elem, emin, emax] : zip(rhs, min, max))
			(elem < -epsilon ? emin : emax) += elem;
		return *this;
	}
	
	bool aabb::contains(const vec3& point) const
	{
		for (auto [elem, emin, emax] : zip(point, min, max))
			if (elem < emin || elem > emax)
				return false;

		return true;
	}

	bool aabb::contains(const aabb& box) const
	{
		for (auto [mymin, mymax, yourmin, yourmax] : zip(min, max, box.min, box.max))
		{
			if (mymin > yourmin || yourmax > mymax)
				return false;
		}

		return true;
	}

	bool aabb::overlaps(const aabb& box) const
	{
		for (auto [mymin, mymax, yourmin, yourmax] : zip(min, max, box.min, box.max))
		{
			if (mymin > yourmax || yourmin > mymax)
				return false;
		}

		return true;
	}
	aabb& aabb::operator*=(const mat4& tfm)
	{
		auto old_sz = extents();
		auto new_center = vec3{ tfm * vec4{ center(), 1 } };

		for (auto [elem, col]: zip(old_sz, tfm))
			elem *= col.length();

		min = new_center - old_sz;
		max = new_center + old_sz;

		return *this;
	}
	aabb aabb::operator*(const mat4& tfm) const
	{
		auto copy = *this;
		return copy *= tfm;
	}
}
