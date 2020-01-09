#pragma once

#include <idk.h>
#include <core/Component.h>
#include <gfx/RenderObject.h>
#include <gfx/CameraFrustum.h>
#include <gfx/CameraClear.h>
#include <math/rect.h>
#include <common/LayerMask.h>

namespace idk
{

	class Camera
		: public Component<Camera>
	{
	public:
		bool enabled = true;

		//real aspect = 16.f / 9.f;
		real near_plane = 0.1f;
		real far_plane = 100.f;

		int  depth = 0;
		RscHandle<RenderTarget> render_target;

		// perspective settings
		rad  field_of_view = deg{ 45.f };

		// orthographic settings
		bool is_orthographic = false;
		real orthographic_size = 1.f;

		CameraClear clear;
		rect viewport;

        LayerMask layer_mask{ 0xFFFFFFFF };

		void LookAt(vec3 target_point, vec3 up = vec3{ 0, 1, 0 });

		vec3	  currentPosition() const;
		vec3	  currentDirection() const;

		Frustum   getFrustum() const;

		mat4      getTightProjection() const;

		vec3 getTightProjectionCenter() const;

		mat4	  ViewMatrix() const;
		mat4	  ProjectionMatrix() const;

		CameraData GenerateCameraData() const;
		float      AspectRatio()const;
	private:
		//Basic controls parameter

		mat4 _viewMatrix;
		mat4 _projectionMatrix;
	};
}