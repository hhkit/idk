#pragma once
#include <idk.h>
#include <core/Component.h>
#include <gfx/RenderObject.h>

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
		RscHandle<RenderTarget> render_target;
		vec4 clear_color{ 0,0,0,1 };

		// perspective settings
		rad  field_of_view = deg{ 45.f };

		// orthographic settings
		bool is_orthographic = false;
		real orthographic_size = 1.f;

		void LookAt(vec3 target_point, vec3 up = vec3{ 0, 1, 0 });

		void LookAt();

		void Focus();

		vec3	  currentPosition() const;
		vec3	  currentTarget() const;
		vec3	  currentDirection() const;
		Ray		  ViewportPointToRay(const vec3& pos);
		Ray		  ViewportPointToRay(const vec2& pos);
		
		//ArcBall rotation
		void	  RotateArcBallCamera(const vec2& screenpos);
		void	  StopRotatingArcBallCamera();
		void	  StartRotatingArcballCamera();

		void	  SetTarget(const vec3& pos);

		mat4 ViewMatrix() const;
		mat4 ProjectionMatrix() const;

		CameraData GenerateCameraData() const;

		Handle<Transform> _selected_target;
	private:
		vec3 _target{0.0f,0.0f,0.0f};
		float _fov;

		//Basic controls parameter
		bool _focusing{ false };
		bool _arcBallRotating{ false };
		bool _rotating{ false };
		vec2 _oldScreenPos;

		real _alpha;
		real _beta;

		vec3 initialVector;
		vec3 currVector;

		real _radius{};

		mat4 _viewMatrix;
		mat4 _projectionMatrix;

		vec3 convertMouseCoord(const vec2&);
	};
}