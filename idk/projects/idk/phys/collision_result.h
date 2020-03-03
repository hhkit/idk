#pragma once
#include <idk.h>
#include <ds/result.h>
#include <phys/collision_contact.h>

namespace idk::phys
{

	bool epsilon_equal(real lhs, real rhs);
	std::pair<vec3, vec3> ClosestPairPointsLineSegmentToLineSegment(const vec3& lineA_1, const vec3& lineA_2, const vec3& lineB_1, const vec3& lineB_2); //need to move this declaration somewhere more fitting. else get linking error
	struct col_success
	{
		vec3 centerA{0.0f};
		vec3 centerB{0.0f};

		vec3 normal{ 0.0f };								// From A to B
		vec3 contact_centroid{ 0.0f };						// centroid of all the contact points

		ContactPoint contacts[8]{};
		int contactCount{ 0 };

		float max_penetration{ 0.0f };

		vec3 tangentVectors[2]{ vec3{0.0f}, vec3{0.0f} };	// Tangent vectors
		

		col_success operator-() const;
	};

	struct col_failure
	{
		real perp_dist        { -1.0f }; // default false.
		vec3 separating_axis  {};

		col_failure operator-() const;
	};

	struct col_result
		: public monadic::result<col_success, col_failure>
	{
	private:
		using Base = monadic::result<col_success, col_failure>;
	public:
		using Base::Base;      // use result's ctors
		using Base::operator=; // use result's assignment

		col_result operator-() const;
	};
}