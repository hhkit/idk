#pragma once
#include <idk.h>
#include <core/Component.h>
#include <shape/aabb.h>
#include <shape/sphere.h>
#include <shape/capsule.h>

namespace idk
{
	class Collider
		: public Component<Collider>
	{
	public:
		using Shapes = std::variant<sphere, capsule>;

		Shapes shape;

		real contact_offset = epsilon;
		bool enabled        = true;
		bool is_trigger     = false;

		aabb bounds() const;
	private:
		Handle<class RigidBody> rigid_body;
		friend class CollisionSystem;
	};
}