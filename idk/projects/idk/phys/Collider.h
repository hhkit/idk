#pragma once
#include <idk.h>
#include <core/Component.h>
#include <phys/collidable_shapes.h>
namespace idk
{
	class Collider
		: public Component<Collider, false>
	{
	public:
		CollidableShapes shape;

		real contact_offset = epsilon;
		bool enabled        = true;
		bool is_trigger     = false;

		real bounciness       { 0.f };
		real static_friction  { .6f };
		real dynamic_friction { .6f };

		bool is_enabled_and_active() const;
		aabb bounds() const;
		void setup_predict();
	private:
		Handle<class RigidBody> _rigidbody;
		aabb _broad_phase;
		friend class PhysicsSystem;
	};
}