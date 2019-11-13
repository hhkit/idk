#include "stdafx.h"
#include <phys/raycasts/collision_raycast.h>
namespace idk::phys
{
	bool epsilon_equal(real lhs, real rhs)
	{
		//Gotta handle infinity
		return lhs == rhs || abs(abs(rhs) - abs(lhs)) <= epsilon;
	}

	//Given point and a line defined by seg_a and seg_b, return the closest point on the line. This is clamped on the line
	vec3 ClosestPtPointSegment(const vec3& point, const vec3& seg_a, const vec3& seg_b) {
		const vec3 line_vector = seg_b - seg_a;
		float t = dot(point - seg_a, line_vector) / dot(line_vector, line_vector);

		if (t < 0) t = 0;
		if (t > 1) t = 1;

		return vec3{ seg_a + t * line_vector };

	}


	//A + B. Could be greatly optimized with std::hash<vec2> + unordered_set and templated
	std::vector<vec3> MinkowskiAddition(const std::vector<vec3> set_a, const std::vector<vec3> set_b) {
		std::vector<vec3> output{};
		for (const auto& i : set_a) {
			for (const auto& j : set_b) {
				auto total = i + j;
				bool shouldAdd = true;
				for (const auto& k : output) { //This is bad juju
					if (k == total) {
						shouldAdd = false;
						break;
					}
				}
				if (shouldAdd)
					output.push_back(total);
			}
		}

		return output;
	}

	//A + B. Could be greatly optimized with std::hash<vec2> + unordered_set and templated
	std::vector<vec3> MinkowskiDifference(const std::vector<vec3> set_a, const std::vector<vec3> set_b) {
		std::vector<vec3> output{};
		for (const auto& i : set_a) {
			for (const auto& j : set_b) {
				auto total = i - j;
				bool shouldAdd = true;
				for (const auto& k : output) { //This is bad juju
					if (k == total) {
						shouldAdd = false;
						break;
					}
				}
				if (shouldAdd)
					output.push_back(total);
			}
		}

		return output;
	}

	//Given points and a direction, return the point furthest from the direction
	const vec3& SupportingPoint2D(const vector <vec3> points, vec3 direction) {
		auto dotValue = direction.dot(points[0]); //Use first points as reference
		auto index = 0;
		for (auto i = 1; i < points.size(); ++i) {
			auto check = direction.dot(points[i]);
			if (dotValue < check) {
				index = i;
				dotValue = check;
			}
		}
		return points[index];
	}
	//Given a bunch of points, give two points closest to origin
	std::pair<vec3,vec3> GJKAlgorithm(const vector<vec3> points) {
		(void)points;
	}

}