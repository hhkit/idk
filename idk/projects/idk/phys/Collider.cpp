#include "stdafx.h"
#include "Collider.h"

namespace idk
{
	aabb Collider::bounds() const
	{
		return std::visit([](const auto& shape) { return shape.bounds(); }, shape);
	}
}
