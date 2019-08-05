#include "stdafx.h"
#include "Transform.h"
#include <math/matrix_transforms.h>

namespace idk
{
	mat4 Transform::GetMatrix() const
	{
		return math::translate(position) * mat4 { s_cast<mat3>(rotation)* math::scale(scale) };
	}
	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}