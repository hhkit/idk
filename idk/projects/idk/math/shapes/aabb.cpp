#include "stdafx.h"
#include "aabb.h"
#include <ds/zip.inl>

namespace idk
{
	vec3 aabb::center() const
	{
		return (min + max) / 2;
	}

	vec3 aabb::extents() const
	{
		return max - min;
	}

	vec3 aabb::halfextents() const
	{
		return extents() / 2;
	}

	float aabb::volume() const
	{
		const auto size = extents();
		return size.x * size.y * size.z;
	}

	const aabb& aabb::bounds() const
	{
		return *this;
	}

	aabb aabb::combine(const aabb& rhs) const
	{
		return aabb{ vec3{ std::min(min.x, rhs.min.x), std::min(min.y, rhs.min.y), std::min(min.z, rhs.min.z) },
					 vec3{ std::max(max.x, rhs.max.x), std::max(max.y, rhs.max.y), std::max(max.z, rhs.max.z) }};
	}

	bool aabb::equals(const aabb& rhs, float)
	{
		return min == rhs.min && max == rhs.max;
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
		auto mintr = min.begin();
		auto endtr = min.end();
		auto maxtr = max.begin();

		auto rmintr = rhs.min.begin();
		auto rmaxtr = rhs.max.begin();

		while (mintr != endtr)
		{
			*mintr = std::min(*mintr, *rmintr);
			*maxtr = std::max(*maxtr, *rmaxtr);

			++mintr;
			++maxtr;
			++rmintr;
			++rmaxtr;
		}

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

	bool aabb::contains(const aabb& other) const
	{
		return	other.min.x >= min.x &&
				other.max.x <= max.x &&
				other.min.y >= min.y &&
				other.max.y <= max.y &&
				other.min.z >= min.z &&
				other.max.z <= max.z;
	}

	bool aabb::overlaps(const aabb& rhs) const
	{
		auto mintr = min.begin();
		auto endtr = min.end();
		auto maxtr = max.begin();

		auto rmintr = rhs.min.begin();
		auto rmaxtr = rhs.max.begin();

		while (mintr != endtr)
		{
			auto& mymin = *mintr;
			auto& mymax = *maxtr;
			auto& yourmin = *rmintr;
			auto& yourmax = *rmaxtr;

			if (mymin > yourmax || yourmin > mymax)
				return false;

			++mintr;
			++maxtr;
			++rmintr;
			++rmaxtr;
		}


		return true;
	}
	aabb& aabb::operator*=(const mat4& tfm)
	{
		auto old_sz = extents();
		const auto new_center = vec3{ tfm * vec4{ center(), 1 } };

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
