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
		void ApplyGravity     (span<class RigidBody> rbs);
		void PredictTransform (span<class RigidBody> rbs,      span<const class Transform> transforms);
		void CollideObjects   (span<class Collider> colliders, span<const class Transform>);
		//void ResolveCollisions(span<class RigidBody> rbs);
		void MoveObjects      (span<class RigidBody> rbs,      span<class Transform> transforms);
	private:
		struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };
		struct pair_hasher   { size_t operator()(const CollisionPair&) const; };

		hash_table<CollisionPair, phys::col_success, pair_hasher> collisions;
		void Init() override;
		void Shutdown() override;
	};
}