#pragma once
#include <idk.h>
#include <phys/Collision_utils.h>

namespace idk
{
	

	struct VelocityState
	{
		vec3 w;
		vec3 v;
	};

	class ContactSolver
	{
	public:
		void Initialize(const CollisionList& collisions);
		void PreSolve(float dt);
		void Solve(float dt);
	private:
		void ApplyGravity();

		int _iterations{ 5 };
		// vector<ContactConstraintState> constraint_states;
		// vector<VelocityState> velocities;
		friend class PhysicsSystem;
	};
}