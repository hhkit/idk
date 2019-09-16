#include "stdafx.h"
#include "Collider.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	aabb Collider::bounds() const
	{
		return std::visit([&](const auto& shape) { return (shape * GetGameObject()->Transform()->GlobalMatrix()).bounds(); }, shape);
	}
}
