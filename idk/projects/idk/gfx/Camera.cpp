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

	mat4 Camera::getTightOrthoProjection() const
	{
		Frustum frustum{ ProjectionMatrix() * ViewMatrix() };

		/*const auto tfm = GetGameObject()->Transform();
		vec3 front = tfm->Forward();
		vec3 right = front.cross(vec3(0,0,1)).normalize();
		vec3 newUp = front.cross(right).normalize();*/
		mat4 rotMat = ViewMatrix();

		array<vec3,8> frustumVert;

		vec3 minV, maxV;
		minV = maxV = rotMat * vec4(frustum.vertices[0], 0);
		unsigned i = 0;
		for (auto& elem : frustumVert)
		{
			elem = rotMat * vec4(frustum.vertices[i],0);
			
			minV.x = std::min(minV.x, elem.x);
			minV.y = std::min(minV.y, elem.y);
			minV.z = std::min(minV.z, elem.z);
			maxV.x = std::max(maxV.x, elem.x);
			maxV.y = std::max(maxV.y, elem.y);
			maxV.z = std::max(maxV.z, elem.z);
		}

		vec3 extent = (maxV - minV) * 0.5f;
		

		return ortho(-extent.x, extent.y, -extent.y, extent.y, -extent.z, extent.z);
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		const auto tfm = GetGameObject()->Transform();
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
		return CameraData{
			GetGameObject(),
			layer_mask,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target,
			false,
			clear,
			Mesh::defaults[MeshType::Box],
			viewport
		};
	}
	float Camera::AspectRatio() const
	{
		return ((render_target)? render_target->AspectRatio():(16.0f/9.0f)) * ((float) viewport.size.x / viewport.size.y);
	}
}
