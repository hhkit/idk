#include "stdafx.h"
#include "Transform.h"

namespace idk
{
	void Transform::CleanRotation()
	{
		rotation.normalize();
	}
}