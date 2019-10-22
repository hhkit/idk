#pragma once
#include <compare>
#include <idk.h>
#include <core/ISystem.h>
#include <phys/collision_result.h>
#include <phys/raycasts/collision_raycast.h>

namespace idk
{
	class PhysicsSystem
		: public ISystem
	{
	public:
		void PhysicsTick            (span <class RigidBody> rbs, span<class Collider> colliders, span<class Transform>);
		void DebugDrawColliders     (span<class Collider> colliders);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList, vector<phys::raycast_result>& ray_resultList);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList);
	private:
		struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };
		struct pair_hasher   { size_t operator()(const CollisionPair&) const; };

		hash_table<CollisionPair, phys::col_success, pair_hasher> collisions;
		void Init() override;
		void Shutdown() override;
	};
}