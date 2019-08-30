#include "stdafx.h"
#include "Camera.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

namespace idk
{
	void Camera::LookAt(vec3 target_point, vec3 up)
	{
		auto tfm = GetGameObject()->Transform();
		
		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), target_point, up)));
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		auto retval = orthonormalize(mat);
		retval[3] = mat[3];
		return invert_rotation(retval);
	}

	mat4 Camera::ProjectionMatrix() const
	{
		return is_orthographic 
			? ortho(-orthographic_size, +orthographic_size, -orthographic_size * aspect, +orthographic_size * aspect, near_plane, far_plane)
			: perspective(field_of_view, aspect, near_plane, far_plane);
	}
}
