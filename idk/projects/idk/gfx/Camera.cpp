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

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), target_point, up).inverse()));
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

		vec3 upvector = tfm->Up();
		vec3 rightvector = tfm->Right();
		vec3 forwardvector = tfm->Forward();

		//mat4 matrix;

		//matrix[0] = vec4(rightvector,0);
		//matrix[1] = vec4(upvector,0);
		//matrix[2] = vec4(forwardvector, 0);
		//matrix[3] = vec4(tfm->GlobalPosition(),1);

		//auto retval1 = orthonormalize(matrix);

		//auto retval1 = matrix.transpose();

		//ProjectionMatrix();

		return retval;
	}

	mat4 Camera::ProjectionMatrix() const
	{
		return is_orthographic
			? ortho(-orthographic_size, +orthographic_size, -orthographic_size * render_target->AspectRatio(), +orthographic_size * render_target->AspectRatio(), near_plane, far_plane)
			: perspective(field_of_view, render_target->AspectRatio(), near_plane, far_plane);
	}
	CameraData Camera::GenerateCameraData() const
	{
		return CameraData{
			0xFFFFFFF,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target
		};
	}
}
