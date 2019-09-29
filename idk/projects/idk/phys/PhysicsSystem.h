#pragma once
#include <compare>
#include <idk.h>
#include <core/ISystem.h>
#include <phys/collision_result.h>

namespace idk
{
	class PhysicsSystem
		: public ISystem
	{
	public:
		void PhysicsTick       (span <class RigidBody> rbs, span<class Collider> colliders, span<class Transform>);
		void DebugDrawColliders(span<class Collider> colliders);
	private:
		struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };
		struct pair_hasher   { size_t operator()(const CollisionPair&) const; };

		hash_table<CollisionPair, phys::col_success, pair_hasher> collisions;
		void Init() override;
		void Shutdown() override;
	};
}