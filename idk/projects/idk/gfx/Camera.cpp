#include "stdafx.h"
#include "Camera.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>
#include <gfx/Mesh.h>
#include <gfx/RenderTarget.h>
#include <res/ResourceHandle.inl>

namespace idk
{
	void Camera::LookAt(vec3 target_point, vec3 up)
	{
		const auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->position, target_point, up)).normalize());
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		//const auto tfm = GetGameObject()->Transform();
		auto retval = orthonormalize(mat);
		retval[3] = mat[3];

		return retval.inverse();
	}
	mat4 ortho_correction(mat4 m)
	{
		m[2][2] = -m[2][2];
		return m;
	}
	mat4 Camera::ProjectionMatrix() const
	{
		return is_orthographic
			? ortho_correction(ortho(-orthographic_size * AspectRatio(), +orthographic_size * AspectRatio(), -orthographic_size , +orthographic_size, near_plane, far_plane))
			: perspective(field_of_view, AspectRatio(), near_plane, far_plane);
	}

	frustum Camera::GetFrustum() const
	{
		return camera_vp_to_frustum(ProjectionMatrix() * ViewMatrix());
	}

	float Camera::AspectRatio() const
	{
		return (render_target ? render_target->AspectRatio() : 16.0f / 9.0f) * viewport.size.x / viewport.size.y;
	}

	CameraData Camera::GenerateCameraData() const
	{	
		rad vfov{  field_of_view * 0.5f };
		rad hfov{ (field_of_view * AspectRatio()) * 0.5f };
		return CameraData{
			GetGameObject(),
			layer_mask,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target,
			enabled,
			clear,
			Mesh::defaults[MeshType::Box],
			viewport,
			near_plane,
			far_plane,
			GetGameObject()->Transform()->Forward(),
			GetGameObject()->Transform()->position,
			{tan(hfov),tan(vfov) },
			field_of_view,
			AspectRatio(),
			gamma,
			ViewMatrix().inverse()
		};
	}
}
