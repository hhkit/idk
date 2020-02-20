#pragma once
#include <idk.h>
#include <phys/AAbbTree.h>

namespace idk
{
	class CollisionManager
	{
	public:
		// Will also cache all dynamic bodies for TestCollisions.
		void UpdatePairs(span<class RigidBody> rbs, span<class Collider> colliders, span<class Transform>);
		void TestCollisions();
		void Reset();

		void DebugDraw();
	private:
		void BuildStaticTree();
		void BuildStaticTree(span<Collider> colliders);

		vector<ColliderInfo> _dynamic_info;

		bool _first_update{ true };
		AabbTree _static_broadphase;

		friend class PhysicsSystem;
	};
}
