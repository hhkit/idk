#include "stdafx.h"
#include "Collider.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <phys/RigidBody.h>

namespace idk
{
	void Collider::find_rigidbody()
	{
		//if (!_rigidbody)
		//{
			auto search_go = GetGameObject();
			while (search_go && !_rigidbody)
			{
				_rigidbody = search_go->GetComponent<RigidBody>();
				search_go = search_go->Parent();
			}
		//}
		_static_cache = !_rigidbody;
	}
	bool Collider::is_static() const
	{
		return !is_trigger && !s_cast<bool>(_rigidbody);
	}
	bool Collider::is_enabled_and_active() const
	{
		return enabled && GetGameObject()->ActiveInHierarchy();
	}
	aabb Collider::bounds() const
	{
		return std::visit([&](const auto& shape) { return (shape * GetGameObject()->Transform()->GlobalMatrix()).bounds(); }, shape);
	}
}
