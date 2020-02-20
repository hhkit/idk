#pragma once
#include <idk.h>


namespace idk::phys
{
	class ContactSolver
	{
	public:
		void Initialize();
		void PreSolve(float dt);
		void Solve(float dt);
	private:
		int _iterations{ 1 };
		friend class PhysicsSystem;
	};
}