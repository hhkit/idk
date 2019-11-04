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
		const mat4 tfm = translate(oriented_box.center) * mat4 { oriented_box.axes() * scale(oriented_box.extents) };
		Draw(Mesh::defaults[MeshType::Box], tfm, c, duration, depth_test);
	}

	void DebugRenderer::Draw(const capsule& capsule, const color& c, seconds duration, bool depth_test)
	{
		//T * R * S
		vec3 rightVec = capsule.dir;
		float i = -rightVec.x;
		rightVec.x = rightVec.y;
		rightVec.y = i;
		rightVec.normalize();
		vec3 frontVec = capsule.dir;
		i = -frontVec.y;
		frontVec.y = frontVec.z;
		frontVec.z = i;

		float minSpherePoint = ((capsule.height * 0.5f) - capsule.radius);
		minSpherePoint = minSpherePoint < 0 ? 0 : minSpherePoint;
		const auto capsule_sphere_top_tfm = translate(capsule.center+ (capsule.dir.get_normalized()* minSpherePoint)) * mat4 { scale(vec3(capsule.radius*2)) };
		const auto capsule_sphere_bot_tfm = translate(capsule.center- (capsule.dir.get_normalized()* minSpherePoint)) * mat4 { scale(vec3(capsule.radius*2)) };

		const vec3 ptOrigin			= capsule.center - (capsule.dir.get_normalized()* capsule.height*0.5f);
		const vec3 ptVelocity		= capsule.dir.get_normalized() * capsule.height;
		const vec3 ptRightOrigin	= ptOrigin + rightVec * capsule.radius;
		const vec3 ptLeftOrigin		= ptOrigin - rightVec * capsule.radius;
		const vec3 ptFrontOrigin	= ptOrigin + frontVec * capsule.radius;
		const vec3 ptBackOrigin		= ptOrigin - frontVec * capsule.radius;
		float sideVelocity = ptVelocity.length() * 0.5f - capsule.radius  < 0 ? 0 : ptVelocity.length() * 0.5f - capsule.radius ;

		const auto capsule_mid_line_tfm		= look_at(ptOrigin		+ ptVelocity * 0.5f, ptOrigin		+ ptVelocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ ptVelocity.length() * 0.5f }) };
		const auto capsule_right_line_tfm	= look_at(ptRightOrigin + ptVelocity * 0.5f, ptRightOrigin	+ ptVelocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ sideVelocity }) };
		const auto capsule_left_line_tfm	= look_at(ptLeftOrigin	+ ptVelocity * 0.5f, ptLeftOrigin	+ ptVelocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ sideVelocity }) };
		const auto capsule_front_line_tfm	= look_at(ptFrontOrigin + ptVelocity * 0.5f, ptFrontOrigin	+ ptVelocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ sideVelocity }) };
		const auto capsule_back_line_tfm	= look_at(ptBackOrigin	+ ptVelocity * 0.5f, ptBackOrigin	+ ptVelocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ sideVelocity }) };
		Draw(Mesh::defaults[MeshType::Circle], capsule_sphere_top_tfm, c, duration, depth_test);
		Draw(Mesh::defaults[MeshType::Circle], capsule_sphere_bot_tfm, c, duration, depth_test);
		Draw(Mesh::defaults[MeshType::Sphere], capsule_sphere_top_tfm, c, duration, depth_test);   //Gets wonky when rotated
		Draw(Mesh::defaults[MeshType::Sphere], capsule_sphere_bot_tfm, c, duration, depth_test);   //Gets wonky when rotated
		Draw(Mesh::defaults[MeshType::Line], capsule_mid_line_tfm, c, duration, depth_test);
		Draw(Mesh::defaults[MeshType::Line], capsule_right_line_tfm, c, duration, depth_test);	   //Gets wonky when rotated
		Draw(Mesh::defaults[MeshType::Line], capsule_left_line_tfm, c, duration, depth_test);	   //Gets wonky when rotated
		Draw(Mesh::defaults[MeshType::Line], capsule_front_line_tfm, c, duration, depth_test);	   //Gets wonky when rotated
		Draw(Mesh::defaults[MeshType::Line], capsule_back_line_tfm, c, duration, depth_test);	   //Gets wonky when rotated

	}

	void DebugRenderer::Draw(const ray& ray, const color& c, seconds duration, bool depth_test)
	{
		//mat4 tfm = translate(ray.origin) * mat4(rotate(ray.direction))
		const auto line_tfm = look_at(ray.origin + ray.velocity / 2, ray.origin + ray.velocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ ray.velocity.length() / 2 }) };
		Draw(Mesh::defaults[MeshType::Line], line_tfm, c, duration, depth_test);

		const auto orient_tfm = orient(ray.velocity.get_normalized());
		const auto arrow_tfm = translate(ray.origin + ray.velocity) * mat4{ orient(ray.velocity.get_normalized()) * scale(vec3{ 0.025f }) };
		Draw(Mesh::defaults[MeshType::Tetrahedron], arrow_tfm, c, duration, depth_test);
		
	}

	void DebugRenderer::Draw(const vec3& start, const vec3& end, const color& c, seconds duration, bool depth_test)
	{
		const vec3 velocity = end - start;
		const auto line_tfm = look_at(start + velocity / 2, start + velocity, vec3{ 0,1,0 }) * mat4 { scale(vec3{ velocity.length() / 2 }) };
		Draw(Mesh::defaults[MeshType::Line], line_tfm, c, duration, depth_test);
	}

	void DebugRenderer::Draw(const sphere& sphere, const color& c, seconds duration, bool depth_test)
	{
		const mat4 tfm = translate(sphere.center) * mat4 { scale(vec3{ sphere.radius * 2 }) };
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