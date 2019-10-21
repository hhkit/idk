#include "stdafx.h"
#include "CameraFrustum.h"

namespace idk {
	Frustum::Frustum()
	{
		unsigned i = 0;
		boundingBox.max = boundingBox.min = vec4(frustumVertList.at(0), 0);

		for (auto& elem : vertices)
		{
			elem = vec4(frustumVertList.at(i), 0.f);
			++i;
			boundingBox.min.x = std::min(boundingBox.min.x, elem.x);
			boundingBox.min.y = std::min(boundingBox.min.y, elem.y);
			boundingBox.min.z = std::min(boundingBox.min.z, elem.z);

			boundingBox.max.x = std::max(boundingBox.max.x, elem.x);
			boundingBox.max.y = std::max(boundingBox.max.y, elem.y);
			boundingBox.max.z = std::max(boundingBox.max.z, elem.z);
		}
	}
	Frustum::Frustum(const mat4& vp)
	{
		const mat4 in_vp = vp.inverse();

		unsigned i = 0;
		boundingBox.max = boundingBox.min = in_vp * vec4(frustumVertList.at(0), 0);

		for (auto& elem : vertices)
		{
			elem = in_vp * vec4(frustumVertList.at(i), 0.f);
			++i;
			boundingBox.min.x = std::min(boundingBox.min.x, elem.x);
			boundingBox.min.y = std::min(boundingBox.min.y, elem.y);
			boundingBox.min.z = std::min(boundingBox.min.z, elem.z);

			boundingBox.max.x = std::max(boundingBox.max.x, elem.x);
			boundingBox.max.y = std::max(boundingBox.max.y, elem.y);
			boundingBox.max.z = std::max(boundingBox.max.z, elem.z);
		}
	}
	Frustum::Frustum(const Frustum& rhs)
		:vertices{ rhs.vertices },
		boundingBox{rhs.boundingBox}
	{}
	Frustum::Frustum(Frustum&& rhs)
		: vertices{ std::move(rhs.vertices) },
		boundingBox{ std::move(rhs.boundingBox) }
	{}
	Frustum& Frustum::operator=(const Frustum& rhs)
	{
		// TODO: insert return statement here
		vertices = rhs.vertices;
		boundingBox = rhs.boundingBox;

		return *this;
	}
	Frustum& Frustum::operator=(Frustum&& rhs)
	{
		// TODO: insert return statement here
		vertices = std::move(rhs.vertices);
		boundingBox = std::move(rhs.boundingBox);

		return *this;
	}
	void Frustum::computeBounding(const mat4& vp)
	{
		const mat4 in_vp = vp.inverse();

		unsigned i = 0;
		boundingBox.max = boundingBox.min = in_vp * vec4(frustumVertList.at(0),0);

		for (auto& elem : vertices)
		{
			elem = in_vp * vec4(frustumVertList.at(i), 0.f);
			++i;
			boundingBox.min.x = std::min(boundingBox.min.x, elem.x);
			boundingBox.min.y = std::min(boundingBox.min.y, elem.y);
			boundingBox.min.z = std::min(boundingBox.min.z, elem.z);

			boundingBox.max.x = std::max(boundingBox.max.x, elem.x);
			boundingBox.max.y = std::max(boundingBox.max.y, elem.y);
			boundingBox.max.z = std::max(boundingBox.max.z, elem.z);
		}
	}
}