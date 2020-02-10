#pragma once
#include <idk.h>


namespace idk::phys
{
	struct ColliderInfo;

	struct ContactPoint
	{
		vec3 position;				// World coordinate of contact
		float penetration;			// Depth of penetration from collision
		float normalImpulse;		// Accumulated normal impulse
		float tangentImpulse[2];	// Accumulated friction impulse
		float bias;					// Restitution + baumgarte
		float normalMass;			// Normal constraint mass
		float tangentMass[2];		// Tangent constraint mass
	};

	struct Manifold
	{
		ColliderInfo* lhs = nullptr, *rhs = nullptr;
		vec3 normal;				// From A to B
		vec3 tangentVectors[2];	// Tangent vectors
		ContactPoint contacts[8];
		int contactCount;
	};

}