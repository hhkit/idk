#pragma once
#include <idk.h>


namespace idk::phys
{
	struct ColliderInfo;

	struct ContactPoint
	{
		vec3 position{ 0, 0, 0 };				// World coordinate of contact
		float penetration{0};			// Depth of penetration from collision
		float normalImpulse{ 0 };		// Accumulated normal impulse
		float tangentImpulse[2]{ 0 };	// Accumulated friction impulse
		float bias{ 0 };					// Restitution + baumgarte
		float normalMass{ 0 };			// Normal constraint mass
		float tangentMass[2];		// Tangent constraint mass
	};

	struct Manifold
	{
		ColliderInfo* lhs = nullptr, *rhs = nullptr;
		vec3 normal{0.0f};				// From A to B
		vec3 tangentVectors[2]{ vec3{0.0f}, vec3{0.0f} };	// Tangent vectors
		ContactPoint contacts[8];
		int contactCount{0};
	};

}