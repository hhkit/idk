#pragma once
#include <idk.h>
#include <core/Component.h>
#include <math/shapes/aabb.h>
#include <math/shapes/box.h>
#include <math/shapes/sphere.h>
#include <math/shapes/capsule.h>

namespace idk
{
	class Collider
		: public Component<Collider>
	{
	public:
		using Shapes = variant<box, sphere, capsule>;

		Shapes shape;

		real contact_offset = epsilon;
		bool enabled        = true;
		bool is_trigger     = false;

		aabb bounds() const;
	private:
		Handle<class RigidBody> _rigid_body;
		friend class CollisionSystem;
	};
}