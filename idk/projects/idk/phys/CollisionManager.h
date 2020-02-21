#pragma once
#include <idk.h>
#include <phys/AAbbTree.h>

namespace idk
{
	class CollisionManager
	{
	public:
		void Init();

		// Will also cache all dynamic bodies for TestCollisions.
		void UpdatePairs(span<class RigidBody> rbs, span<class Collider> colliders);
		void TestCollisions();
		void PreSolve();
		void Solve();
		void Reset();

		void DebugDrawContactPoints(float dt);
		void DebugDrawColliders(span<class Collider> colliders, float dt);
		void DebugDrawTree();

		friend class PhysicsSystem;
	private:
		// for broadphase
		vector<ColliderInfo> _dynamic_info;
		AabbTree _static_broadphase;
		vector<ColliderInfoPair> _info; 

		CollisionList _collisions;

		void BuildStaticTree();
		void BuildStaticTree(span<Collider> colliders);
		phys::col_result CollideShapes(const CollidableShapes& lhs, const CollidableShapes& rhs);

		
	};
}
