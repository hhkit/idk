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

		mat4 LocalMatrix() const;
		mat4 GlobalMatrix() const;

		void GlobalMatrix(const mat4&);

		vec3 GlobalPosition() const;
		quat GlobalRotation() const;
		vec3 GlobalScale() const;

		void GlobalPosition(vec3);
		void GlobalRotation(quat);
		void GlobalScale(vec3);

		void CleanRotation();
	};
}