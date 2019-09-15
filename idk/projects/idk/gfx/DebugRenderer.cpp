#include "stdafx.h"
#include "DebugRenderer.h"

#include <gfx/MeshFactory.h>
#include <math/matrix_transforms.h>

namespace idk
{
	void DebugRenderer::Draw(RscHandle<Mesh> mesh, const mat4& transform, const color& c, seconds duration, bool depth_test)
	{
		debug_info.emplace_back(DebugInfo{ mesh, transform, c, duration, depth_test });
	}
	void DebugRenderer::Draw(const aabb& bounding_box, const color& c, seconds duration, bool depth_test)
	{
		Draw(box{ bounding_box.center(), bounding_box.extents() }, c, duration, depth_test);
	}
	void DebugRenderer::Draw(const box& oriented_box, const color& c, seconds duration, bool depth_test)
	{
		mat4 tfm = translate(oriented_box.center) * mat4 { scale(oriented_box.extents / 2)* oriented_box.axes };
		Draw(Mesh::defaults[MeshType::Box], tfm, c, duration, depth_test);
	}

	void DebugRenderer::Draw(const ray& ray, const color& c, seconds duration, bool depth_test)
	{
		//mat4 tfm = translate(ray.origin) * mat4(rotate(ray.direction))
		assert(false);
	}

	void DebugRenderer::Draw(const sphere& sphere, const color& c, seconds duration, bool depth_test)
	{
		mat4 tfm = translate(sphere.center) * mat4 { scale(vec3{ sphere.radius }) };
		Draw(Mesh::defaults[MeshType::Sphere], tfm, c, duration, depth_test);
	}

	void DebugRenderer::GraphicsTick()
	{
		for (auto& elem : debug_info)
			elem.display_time += Core::GetRealDT();
		
		debug_info.erase(std::remove_if(debug_info.begin(), debug_info.end(), [](auto& elem) -> bool
		{
			return elem.display_time > elem.duration;
		}), debug_info.end());
	}

	span<const DebugRenderer::DebugInfo> DebugRenderer::GetWorldDebugInfo() const
	{
		return span<const DebugRenderer::DebugInfo>{debug_info};
	}
}