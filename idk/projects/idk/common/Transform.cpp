#include "stdafx.h"
#include "Transform.h"
#include <math/matrix_transforms.h>

namespace idk
{
	mat4 Transform::GetMatrix() const
	{
		return translate(position) * mat4 { quat_cast<mat3>(rotation)* idk::scale(scale) };
	}
	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}