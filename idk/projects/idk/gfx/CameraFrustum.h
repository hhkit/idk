#pragma once
#include <idk.h>
#include <math/shapes/aabb.h>

namespace idk
{
	static const vector<vec3> frustumVertList
	{
		{-1.f,1.f,-1.f}, //top left-near
		{1.f,1.f,-1.f},  //top right-near
		{-1.f,-1.f,-1.f}, //bottom left-near
		{1.f,-1.f,-1.f}, //bottom right-near
		{-1.f,1.f,1.f},  //top left-far
		{1.f,1.f,1.f},   //top right-far
		{-1.f,-1.f,1.f}, //bottom left-far
		{1.f,-1.f,1.f}  //bottom right-far
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