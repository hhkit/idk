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
	class Camera
		: public Component<Camera>
	{
	public:
		bool enabled = true;

		//real aspect = 16.f / 9.f;
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

		bool overlay_debug_draw = true;

		opt<RscHandle<CubeMap>> skybox;
		opt<RscHandle<Mesh>>    skybox_mesh;


		void LookAt(vec3 target_point, vec3 up = vec3{ 0, 1, 0 });

		vec3	  currentPosition() const;
		vec3	  currentDirection() const;

		mat4	  ViewMatrix() const;
		mat4	  ProjectionMatrix() const;

		CameraData GenerateCameraData() const;
		float      AspectRatio()const;
	private:	
		float _fov;

		//Basic controls parameter

		mat4 _viewMatrix;
		mat4 _projectionMatrix;
	};
}