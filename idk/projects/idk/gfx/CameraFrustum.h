#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	static const array<vec3,8> frustumVertList
	{
		vec3{-1.f,1.f,-1.f}, //top left-near
		vec3{1.f,1.f,-1.f},  //top right-near
		vec3{-1.f,-1.f,-1.f}, //bottom left-near
		vec3{1.f,-1.f,-1.f}, //bottom right-near
		vec3{-1.f,1.f,1.f},  //top left-far
		vec3{1.f,1.f,1.f},   //top right-far
		vec3{-1.f,-1.f,1.f}, //bottom left-far
		vec3{1.f,-1.f,1.f}  //bottom right-far
	};
	class Frustum
	{
	public:
		Frustum();
		Frustum(const mat4& vp);
		Frustum(const Frustum& rhs);
		Frustum(Frustum&& rhs);
		Frustum& operator=(const Frustum& rhs);
		Frustum& operator=(Frustum&& rhs);

		void computeBounding(const mat4& vp);

		vector<vec3>	vertices{ 8 };
		aabb			boundingBox;
	};
};