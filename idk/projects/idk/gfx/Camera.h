#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	struct CamResult
	{
		vec3 result{};
		bool dirty{};
	};
	struct Ray
	{
		vec3 direction{};
		vec3 origin{};
	};
	class Camera
		: public Component<Camera>
	{
	public:
		bool enabled = true;

		real aspect = 16.f / 9.f;
		real near_plane = 0.1f;
		real far_plane = 10.f;

		int  depth = 0;
		//TODO add render texture  
		vec4 clear_color{ 0,0,0,1 };

		// perspective settings
		rad  field_of_view = rad{ 45.f };

		// orthographic settings
		bool is_orthographic = false;
		real orthographic_size = 1.f;

		void LookAt(vec3 target_point, vec3 up = vec3{ 0, 1, 0 });

		CamResult currentPosition() const;
		CamResult currentTarget() const;
		CamResult currentDirection() const;
		Ray ViewportPointToRay();

		mat4 ViewMatrix() const;
		mat4 ProjectionMatrix() const;
	private:
		vec3 _position;
		vec3 _target;
		vec3 _direction;
		vec3 _upVector;
		float _fov;
		bool _dirty{ false };

		mat4 _viewMatrix;
		mat4 _projectionMatrix;
	};
}