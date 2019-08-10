#pragma once

#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class Transform
		: public Component<Transform>
	{
	public:
		vec3 scale{ 1.f };
		quat rotation;
		vec3 position;

		mat4 GetMatrix() const;
		void CleanRotation();
	};
}