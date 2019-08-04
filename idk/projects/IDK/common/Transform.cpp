#include "stdafx.h"
#include "Transform.h"

namespace idk
{
	mat4 Transform::GetMatrix() const
	{
		return mat4();
	}
	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}