#pragma once
#include <idk.h>
#include <phys/AAbbTree.h>

namespace idk
{
	struct ContactState
	{
		vec3 ra{ 0.0f };					// Vector from C.O.M to contact position
		vec3 rb{ 0.0f };					// Vector from C.O.M to contact position
		float penetration{0.0f};			// Depth of penetration from collision
		float normalImpulse{ 0.0f };		// Accumulated normal impulse
		float tangentImpulse[2]{ 0.0f };	// Accumulated friction impulse
		float bias{ 0.0f };					// Restitution + baumgarte
		float normalMass{ 0.0f };			// Normal constraint mass
		float tangentMass[2]{ 0.0f };		// Tangent constraint mass
	};
	struct ContactConstraintState
	{
		ContactState contacts[8];
		int contactCount{ 0 };
		vec3 tangentVectors[2]{ vec3{0.0f}, vec3{0.0f} };	// Tangent vectors
		vec3 normal{ 0.0f };				// From A to B
		vec3 centerA{ 0.0f };
		vec3 centerB{ 0.0f };
		mat3 iA{};
		mat3 iB{};
		float mA{ 0.0f };
		float mB{ 0.0f };
		float restitution{ 0.0f };
		float friction{ 0.0f };
		RigidBody* rbA{nullptr};
		RigidBody* rbB{ nullptr };

		bool solve = true;
	};

	class CollisionManager
	{
	public:
		void Init();

		// Will also cache all dynamic bodies for TestCollisions.
		void NewFrame(span<class RigidBody> rbs, span<class Collider> colliders);
		void UpdateDynamics();
		void TestCollisions();
		void PreSolve();
		void Solve();
		void Finalize();
		void Reset();

		void DebugDrawContactPoints(float dt);
		void DebugDrawColliders(span<class Collider> colliders, float dt);
		void DebugDrawTree();

		friend class PhysicsSystem;
	private:
		// Detection
		vector<ColliderInfo> _dynamic_info;	// for broadphase
		AabbTree _static_broadphase;		// for broadphase
		vector<ColliderInfoPair> _info;		// for broadphase
		CollisionList _collisions;

		// Resolution
		vector<ContactConstraintState> constraint_states;
		// vector<VelocityState> velocities;

		void BuildStaticTree();
		void BuildStaticTree(span<Collider> colliders);
		phys::col_result CollideShapes(const CollidableShapes& lhs, const CollidableShapes& rhs);

		
	};
}
