#pragma once
#include <idk.h>
#include <phys/Collision_utils.h>

namespace idk
{
	struct ContactSolution
	{
		CollisionPair collision;

	};

	class ContactSolver
	{
	public:
		void Initialize();
		void PreSolve(float dt);
		void Solve(float dt);
	private:
		void ApplyGravity();

		int _iterations{ 5 };
		friend class PhysicsSystem;
	};
}