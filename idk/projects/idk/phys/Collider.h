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
		: public Component<Collider, false>
	{
	public:
		using Shapes = variant<box, sphere, capsule>;

		Shapes shape;

		real contact_offset = epsilon;
		bool enabled        = true;
		bool is_trigger     = false;

		real bounciness       { 0.f };
		real static_friction  { .4f };
		real dynamic_friction { .4f };

		aabb bounds() const;
		void setup_predict();
	private:
		Handle<class RigidBody> _rigidbody;
		aabb _broad_phase;
		friend class PhysicsSystem;
	};
}