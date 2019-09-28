#include "stdafx.h"
#include "Camera.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

#include <gfx/RenderTarget.h>

//Test
#include <app/Application.h>

namespace idk
{
	void Camera::LookAt(vec3 target_point, vec3 up)
	{
		auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->position, target_point, up)).normalize());
	}

	vec3 Camera::currentPosition() const
	{
		auto tfm = GetGameObject()->Transform();

		return tfm->GlobalPosition();
	}

	vec3 Camera::currentDirection() const
	{
		auto tfm = GetGameObject()->Transform();
		//return _direction;
		return GetGameObject()->Transform()->Forward().normalize();
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		auto tfm = GetGameObject()->Transform();
		auto retval = orthonormalize(mat);
		retval[3] = mat[3];

		/*vec3 upvector = tfm->Up();
		vec3 rightvector = tfm->Right();
		vec3 forwardvector = tfm->Forward();

		mat4 findMat = retval.inverse();

		mat4 matrix = mat.transpose();*/

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
		variant<vec4, RscHandle<CubeMap>> m;
		if (skybox.has_value())
		{
			m = *skybox;
		}
		else
			m = clear_color;
		
		return CameraData{
			0xFFFFFFF,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target,
			m,
			skybox_mesh
		};
	}
	float Camera::AspectRatio() const
	{
		return (render_target)? render_target->AspectRatio():(16.0f/9.0f);
	}
}
