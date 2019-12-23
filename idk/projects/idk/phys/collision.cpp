#include "stdafx.h"
#include <phys/raycasts/collision_raycast.h>
#include <math/linear.h>

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

	//Given two lines defined by two points each, return the closest pair on both lines. pair.first will return the point on lineA and pair.second returns the point on lineB
	std::pair<vec3,vec3> ClosestPairPointsLineSegmentToLineSegment( const vec3& lineA_1, const vec3& lineA_2, const vec3& lineB_1, const vec3& lineB_2) {
		//ClosestPoint at first =
		const vec3 vectorA		= lineA_2 - lineA_1;
		const vec3 vectorB		= lineB_2 - lineB_1;
		const vec3 vectorA1B1	= lineA_1 - lineB_1;

		const float sq_lenA = vectorA.length_sq();  //Always more than 0
		const float sq_lenB = vectorB.length_sq();  //Always more than 0
		const float b = vectorA.dot(vectorB);
		const float d = vectorA.dot(vectorA1B1);
		const float e = vectorB.dot(vectorA1B1);


		const float SMALL_NUM = 0.00000001f;		//Avoid division overflow
		const float D = sq_lenA * sq_lenB - b * b;  // always >= 0
		float ratioA{};								//Clamps to 0.0f to 1.0f
		float ratioB{};								//Clamps to 0.0f to 1.0f
		float rangeA{};				// ratioA = rangeA / divideA 
		float rangeB{};				// ratioB = rangeB / divideB 
		float divideA = D;			// Starts at D
		float divideB = D;			// Starts at D
		
		if (D < SMALL_NUM)		// If line is parallel (or closely parallel)
		{ 
			rangeA = 0.0f;			// force using point P0 on segment S1
			divideA = 1.0f;			// Avoids zero division
			rangeB = e;
			divideB = sq_lenB;
		}
		else
		{                 
			// Finds the range for both lines. This is unclamped where the two points may lie outside of both lines. 
			rangeA = (b * e - sq_lenB * d);
			rangeB = (sq_lenA * e - b * d);


			
			if (rangeA < 0.0f)			// Min point on vectorA
			{        
				rangeA = 0.0f;
				rangeB = e;
				divideB = sq_lenB;
			}
			else if (rangeA > divideA)	// Max point on vectorA
			{  
				rangeA = divideA;
				rangeB = e + b;
				divideB = sq_lenB;
			}
		}

		if (rangeB < 0.0f)				// Min point on vectorB
		{            
			rangeB = 0.0f;
			
			if (-d < 0.0f)				//Re-edit rangeA to find new point
				rangeA = 0.0f;
			else if (-d > sq_lenA)
				rangeA = divideA;
			else
			{
				rangeA = -d;
				divideA = sq_lenA;
			}
		}
		else if (rangeB > divideB)		// Max point on vectorA
		{      
			rangeB = divideB;
			
			if ((-d + b) < 0.0f)		//Re-edit rangeA to find new point
				rangeA = 0.0f;
			else if ((-d + b) > sq_lenA)
				rangeA = divideA;
			else
			{
				rangeA = (-d + b);
				divideA = sq_lenA;
			}
		}
		// finally do the division to get sc and tc
		ratioA = (abs(rangeA) < SMALL_NUM ? 0.0f : rangeA / divideA);
		ratioB = (abs(rangeB) < SMALL_NUM ? 0.0f : rangeB / divideB);

		//http://geomalgorithms.com/a07-_distance.html#dist3D_Segment_to_Segment

		return std::pair<vec3, vec3>(lineA_1 + (ratioA * vectorA), lineB_1 + (ratioB * vectorB));
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
        return {};
	}

}