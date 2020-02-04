#include "stdafx.h"
#include "Camera.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <gfx/Mesh.h>
#include <gfx/RenderTarget.h>
#include <res/ResourceHandle.inl>
//Test
#include <app/Application.h>

namespace idk
{
	void Camera::LookAt(vec3 target_point, vec3 up)
	{
		const auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->position, target_point, up)).normalize());
	}

	vec3 Camera::currentPosition() const
	{
		const auto tfm = GetGameObject()->Transform();

		return tfm->GlobalPosition();
	}

	vec3 Camera::currentDirection() const
	{
		const auto tfm = GetGameObject()->Transform();
		//return _direction;
		return GetGameObject()->Transform()->Forward();
	}

	Frustum Camera::getFrustum() const
	{
		return Frustum(ProjectionMatrix()*ViewMatrix());
	}

	mat4 Camera::getTightProjection() const
	{
		//Frustum frustum{ ProjectionMatrix() * ViewMatrix() };

		///*const auto tfm = GetGameObject()->Transform();
		//vec3 front = tfm->Forward();
		//vec3 right = front.cross(vec3(0,0,1)).normalize();
		//vec3 newUp = front.cross(right).normalize();*/
		//mat4 rotMat = ViewMatrix();

		//array<vec3,8> frustumVert;

		//vec3 minV, maxV;
		//minV = maxV = rotMat * vec4(frustum.vertices[0], 0);
		//unsigned i = 0;
		//for (auto& elem : frustumVert)
		//{
		//	elem = rotMat * vec4(frustum.vertices[i],0);
		//	
		//	minV.x = std::min(minV.x, elem.x);
		//	minV.y = std::min(minV.y, elem.y);
		//	minV.z = std::min(minV.z, elem.z);
		//	maxV.x = std::max(maxV.x, elem.x);
		//	maxV.y = std::max(maxV.y, elem.y);
		//	maxV.z = std::max(maxV.z, elem.z);
		//}

		//vec3 extent = (maxV - minV) * 0.5f;
		//

		//return ortho(-extent.x, extent.y, -extent.y, extent.y, -extent.z, extent.z);

		//const auto tfm = GetGameObject()->Transform();
		
		//vec3 far_corners = getFarCorner();
		
		//float end = near_plane + far_plane;
		//vec3 sphere_center = tfm->position + tfm->Forward() * (near_plane + 0.5f * end);

		float sphere_radius = getBSphereRadius();
		//float rad = sphere_radius / 2.f;

		return ortho(-sphere_radius, sphere_radius, -sphere_radius, sphere_radius,-sphere_radius, sphere_radius);
	

	}

	vec3 Camera::getFarCorner() const
	{
		const auto tfm = GetGameObject()->Transform();
		vec3 front = tfm->Forward();
		vec3 right = front.cross(vec3(0, 0, 1)).normalize();
		vec3 newUp = front.cross(right).normalize();

		float ap = AspectRatio();

		vec2 tangent_fov = { tanf(ap * *deg(field_of_view).data()),tanf(ap) };

		return front + right * tangent_fov.x + newUp * tangent_fov.y;
	}

	real Camera::getBSphereRadius() const
	{
		const auto tfm = GetGameObject()->Transform();

		float end = near_plane + far_plane;
		vec3 sphere_center = tfm->position + tfm->Forward() * (near_plane + 0.5f * end);
		
		vec3 far_corners = getFarCorner();
		
		return (tfm->position + far_corners * far_plane - sphere_center).length();
	}

	vec3 Camera::getTightProjectionCenter() const
	{
		const auto tfm = GetGameObject()->Transform();
		vec3 front = tfm->Forward();
		vec3 half_cascade = vec3(front * 0.5f * far_plane);
		return tfm->position + half_cascade;
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		//const auto tfm = GetGameObject()->Transform();
		auto retval = orthonormalize(mat);
		retval[3] = mat[3];

		return retval.inverse();
	}

	mat4 Camera::ProjectionMatrix() const
	{
		return is_orthographic
			? ortho(-orthographic_size, +orthographic_size, -orthographic_size * AspectRatio(), +orthographic_size * AspectRatio(), near_plane, far_plane)
			: perspective(field_of_view, AspectRatio(), near_plane, far_plane);
	}
	CameraData Camera::GenerateCameraData() const
	{	
		//rad vfov{  field_of_view *0.5f };
		//rad hfov{ ( field_of_view  *AspectRatio()) * 0.5f };
		return CameraData{
			GetGameObject(),
			layer_mask,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target,
			false,
			clear,
			Mesh::defaults[MeshType::Box],
			viewport,
			near_plane,
			far_plane,
			GetGameObject()->Transform()->Forward(),
			GetGameObject()->Transform()->position,
			field_of_view,
			AspectRatio()
		};
	}
	float Camera::AspectRatio() const
	{
		return ((render_target)? render_target->AspectRatio():(16.0f/9.0f)) * ((float) viewport.size.x / viewport.size.y);
	}
}
