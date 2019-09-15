#pragma once
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class PhysicsSystem
		: public ISystem
	{
	public:
		void CollideObjects(span<class Collider> colliders, span<const class Transform>);
		void MoveObjects(span<class RigidBody> rbs, span<class Transform> transforms);
	private:
		void Init() override;
		void Shutdown() override;
	};
}