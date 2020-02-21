#pragma once
#include <idk.h>


namespace idk
{
	class ContactSolver
	{
	public:
		void Initialize();
		void PreSolve(float dt);
		void Solve(float dt);
	private:
		void ApplyGravity();

		int _iterations{ 1 };
		friend class PhysicsSystem;
	};
}