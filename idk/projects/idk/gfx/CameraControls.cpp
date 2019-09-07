#include "stdafx.h"

//Graphics
#include "GraphicsSystem.h"
#include <gfx/RenderTarget.h>

//Camera
#include "CameraControls.h"
#include "Camera.h"

//IDK
#include "IDK.h"
#include "Core/GameObject.h"

//Common
#include <common/Transform.h>

#include <test/TestSystem.h>

#include <Core/Component.h>


//Math
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

namespace idk {
	vec3 CameraControls::currentTarget() const
	{
		return _target;
	}
	void CameraControls::FocusOnObj(Handle<GameObject> gameObjToFocus)
	{
		//Handle<Camera> currentCam = current_camera;
		
		auto tfm = gameObjToFocus->GetComponent<Transform>();

		SetTarget(tfm);

		_target = tfm->GlobalPosition();

		Focus();
	}

	void CameraControls::LookAt()
	{
		auto tfm = current_camera->GetGameObject()->Transform();
		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), _target, tfm->Up())).normalize());

	}

	void CameraControls::Focus()
	{
		auto tfm = current_camera->GetGameObject()->Transform();

		
		if (!selected_target)
			current_camera->LookAt(_target, tfm->Up());
		else
		{
			//check for facing direction (same)
			//if (tfm->Forward().dot(selected_target->Forward()) > 0.f)
			tfm->position = vec3{ selected_target->GlobalPosition().x, selected_target->GlobalPosition().y, selected_target->GlobalPosition().z - 2.5f };

			vec3 up = tfm->Up();

			current_camera->LookAt(_target);
		}

		_focusing = true;
	}

	void CameraControls::SetTarget(Handle<Transform> pos)
	{
		selected_target = pos;
		_target = selected_target->GlobalPosition();
	}

	void CameraControls::SetCurrentCamera(Handle<Camera> cCam)
	{
		current_camera = cCam;
	}

	void CameraControls::RotateCamera(const vec2& screenpos)
	{
		if (_rotating)
		{

			/*
			vec2 scrCenter = { 0.5f,
			0.5f };
			_radius = std::min(scrCenter.x, scrCenter.y);


			vec2 screenPt = { (screenpos.x - scrCenter.x),(scrCenter.y - screenpos.y) };

			if (!_arcBallRotating)
			{
				initialVector = convertMouseCoord(screenPt).normalize();

				currVector = initialVector;
				_arcBallRotating = true;
			}
			else
				currVector = convertMouseCoord(screenPt).normalize();

			vec3 rotationAxis = currVector.cross(initialVector.normalize());

			real rotAngle = std::acos(currVector.dot(initialVector));

			auto tfm = current_camera->GetGameObject()->Transform();

			tfm->rotation = (quat{ rotationAxis, deg{rotAngle} } *tfm->rotation).normalize();

			/*/


			_alpha = (screenpos.x - _oldScreenPos.x)*90.f;
			_beta = (screenpos.y - _oldScreenPos.y)*90.f;

			float angle_A = _alpha / 180.f * pi;
			float angle_B = _beta / 180.f * pi;

			angle_A;
			angle_B;

			float maxDeg = 89.f / 180.f * pi;
			maxDeg;

			//mat3 rotMatrix = rotate(vec3(0, 1, 0), rad(angle_A));
			//mat3 rotMatrix2 = rotate(vec3(0, 0, 1), rad(angle_B));

			//mat4 matrix{ rotMatrix * rotMatrix2 };

			//auto tfm = current_camera->GetGameObject()->Transform();

			Ray result = ViewportPointToRay(screenpos);

			//_target += vec3{ vec2(result.origin.xy) * Core::GetDT().count() ,0.f };

			//LookAt();

			//_target = result.origin + result.direction;

			//tfm->GlobalRotation(decompose_rotation_matrix(matrix));
			auto tfm = current_camera->GetGameObject()->Transform();

			tfm->rotation = (quat{ vec3{0,1,0}, deg{_beta}  } *tfm->rotation).normalize();
			tfm->rotation += (quat{ vec3{1,0,0}, deg{_alpha}  } *tfm->rotation).normalize();

			//vec3 uh{_alpha,_beta,1.f};

			//uh = uh.normalize();

			_oldScreenPos = screenpos;

			//*/

		}
	}


	void CameraControls::StopRotatingCamera()
	{
		_rotating = false;
		_arcBallRotating = false;
		//_oldScreenPos = { 0,0 };
	}

	void CameraControls::StartRotatingCamera()
	{
		_rotating = true;
	}

	CameraControls::CameraControls(Handle<Camera> cam)
		:current_camera {cam}
	{
	}

	Ray CameraControls::ViewportPointToRay(const vec3& vp_pos)
	{
		//Current 
		//auto vec = GetGameObject()->Transform()->GlobalPosition();

		//Viewport(0,1) to screen space (NDC)(-1,1) first 
		vec2 screen_pos = { (2.f * vp_pos.x - 1.f) * current_camera->render_target->AspectRatio(), 1.f - 2.f * vp_pos.y };


		//screen space back to 4d clip space 
		vec4 clip_pos = { screen_pos.x,screen_pos.y, -1.f,1.f };


		//Inversing pos from clip space to view space on the near plane
		vec4 viewpoint_pos = current_camera->ProjectionMatrix().inverse() * clip_pos;

		viewpoint_pos.y = -1.f;
		viewpoint_pos.z = 0.f;

		//Now getting the world position of the point
		vec3 world_pos = (current_camera->ViewMatrix().inverse() * viewpoint_pos).xyz;
		world_pos = world_pos.normalize();


		//References for this method (http://antongerdelan.net/opengl/raycasting.html)


		return Ray{ current_camera->currentDirection(),world_pos };
	}

	Ray CameraControls::ViewportPointToRay(const vec2& vp_pos)
	{
		//Current 
		//auto vec = GetGameObject()->Transform()->GlobalPosition();

		//Viewport(0,1) to screen space (NDC)(-1,1) first 
		vec2 screen_pos = { (2.f * vp_pos.x - 1.f) * current_camera->render_target->AspectRatio(), 1.f - 2.f * vp_pos.y };


		//screen space back to 4d clip space 
		vec4 clip_pos = { screen_pos.x,screen_pos.y, -1.f,1.f };


		//Inversing pos from clip space to view space on the near plane
		vec4 viewpoint_pos = current_camera->ProjectionMatrix().inverse() * clip_pos;

		viewpoint_pos.y = -1.f;
		viewpoint_pos.z = 0.f;

		//Now getting the world position of the point
		vec3 world_pos = (current_camera->ViewMatrix().inverse() * viewpoint_pos).xyz;
		world_pos = world_pos.normalize();


		//References for this method (http://antongerdelan.net/opengl/raycasting.html)


		return { current_camera->currentDirection(),world_pos };
	}

	vec3 CameraControls::convertMouseCoord(const vec2& screenCoord)
	{
		real d = screenCoord.x * screenCoord.x + screenCoord.y + screenCoord.y;

		real radiusSq = _radius * _radius;

		//Check if the point is out of the ball radius
		return  (d > radiusSq) ?
			vec3(screenCoord.x, screenCoord.y, 0) :
			vec3(screenCoord.x, screenCoord.y, sqrt(radiusSq - d));

	}
};
