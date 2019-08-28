#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class Camera
		: public Component<Camera>
	{
	public:
		bool enabled    = true;

		real aspect     = 16.f / 9.f;
		real near_plane = 0.1f;
		real far_plane  = 100.f;

		// perspective settings
		rad  field_of_view = deg{ 90.f };

		// orthographic settings
		bool is_orthographic   = false;
		real orthographic_size = 1.f;

		void LookAt(vec3 target_point, vec3 up = vec3{ 0, 1, 0 });
		mat4 ViewMatrix() const;
		mat4 ProjectionMatrix() const;
	};
}