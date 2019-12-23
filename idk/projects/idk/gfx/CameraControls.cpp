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
#include <iostream>


//Math
#include <math/matrix_transforms.inl>
#include <math/matrix_decomposition.inl>

namespace idk {
	vec3 CameraControls::currentTarget() const
	{
		return _target;
	}
	void CameraControls::FocusOnObj(Handle<GameObject> gameObjToFocus)
	{
		//Handle<Camera> currentCam = current_camera;
		
		const auto tfm = gameObjToFocus->GetComponent<Transform>();

		SetTarget(tfm);

		_target = tfm->GlobalPosition();

		Focus();
	}

	void CameraControls::LookAt()
	{
        const auto tfm = current_camera->GetGameObject()->Transform();
		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), _target, tfm->Up())).normalize());
	}

	void CameraControls::Focus()
	{
        const auto tfm = current_camera->GetGameObject()->Transform();

		if (!selected_target)
			current_camera->LookAt(_target, tfm->Up());
		else
		{
			//check for facing direction (same)
			//if (tfm->Forward().dot(selected_target->Forward()) > 0.f)
			tfm->position = vec3{ selected_target->GlobalPosition().x, selected_target->GlobalPosition().y, selected_target->GlobalPosition().z - 2.5f };
			current_camera->LookAt(_target,idk::vec3(0,1,0));
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


			_alpha = (screenpos.x - _oldScreenPos.x)*90.f;
			_beta = (screenpos.y - _oldScreenPos.y)*90.f;

			//float angle_A = _alpha / 180.f * pi;
			//float angle_B = _beta / 180.f * pi;

			//angle_A;
			//angle_B;

			//float maxDeg = 89.f / 180.f * pi;
			//maxDeg;

			//mat3 rotMatrix = rotate(vec3(0, 1, 0), rad(angle_A));
			//mat3 rotMatrix2 = rotate(vec3(0, 0, 1), rad(angle_B));

			//mat4 matrix{ rotMatrix * rotMatrix2 };

			//auto tfm = current_camera->GetGameObject()->Transform();

			//Ray result = ViewportPointToRay(screenpos);

			//_target += vec3{ vec2(result.origin.xy) * Core::GetDT().count() ,0.f };

			//LookAt();

			//_target = result.origin + result.direction;

			//tfm->GlobalRotation(decompose_rotation_matrix(matrix));
			const auto tfm = current_camera->GetGameObject()->Transform();

			//tfm->rotation = (quat{ vec3{0,1,0}, deg{_beta}  } *tfm->rotation).normalize();
			//tfm->rotation += (quat{ vec3{1,0,0}, deg{_alpha}  } *tfm->rotation).normalize();

			quat rotation;
			rotation = (quat{ vec3{0,1,0}, deg{_alpha} } *tfm->rotation).normalize();
			rotation += (quat{ vec3{1,0,0}, deg{_beta} } *tfm->rotation).normalize();


			tfm->GlobalRotation(rotation);

			//vec3 uh{_alpha,_beta,1.f};

			//uh = uh.normalize();

			_oldScreenPos = screenpos;

			//*/

		}
	}

	void CameraControls::PanCamera(const vec2& screenpos)
	{
		if (_panning)
		{
			const real x_vec = (screenpos.x - _oldScreenPos.x);
			const real y_vec = (screenpos.y - _oldScreenPos.y);

			const auto tfm = current_camera->GetGameObject()->Transform();
			//tfm->position += camSpd * (x_vec, 0) * Core::GetRealDT().count() * tfm->Right();
			//tfm->position += camSpd * (0, y_vec) * Core::GetRealDT().count() * tfm->Up();
			const vec2 p = (camSpd * vec2(x_vec, y_vec)) * panSpd * Core::GetRealDT().count();
			const vec3 pos = vec3(p.x,p.y,0).normalize();

			tfm->GlobalPosition(tfm->GlobalPosition()+ pos);

			_oldScreenPos = screenpos;
		}
	}

	void CameraControls::StopRotatingCamera()
	{
		_rotating = false;
		_arcBallRotating = false;
		//_oldScreenPos = { 0,0 };
	}

	void CameraControls::StartRotatingCamera(const vec2& screenpos)
	{
		_rotating = true;
		_oldScreenPos = screenpos;
	}

	void CameraControls::StartPanningCamera(const vec2& screenpos)
	{
		_panning = true;
		_oldScreenPos = screenpos;
	}

	void CameraControls::StopPanningCamera()
	{
		_panning = false;
	}


	CameraControls::CameraControls(Handle<Camera> cam)
		:current_camera {cam}
	{
	}

	ray CameraControls::WindowsPointToRay(const vec3& vp_pos)
	{
		vec2 ndcPos = vp_pos;

		const auto view_mtx = current_camera->ViewMatrix();
		const auto pers_mtx = current_camera->ProjectionMatrix();

		//-1 to 1 (ndc)
		ndcPos -= vec2(0.5f, 0.5f);
		ndcPos /= 0.5f;

		ndcPos = vec2(ndcPos.x, -ndcPos.y);

		vec4 vfPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 0, 1);
		vfPos *= 1.f / vfPos.w;
		vec4 wfPos = view_mtx.inverse() * vfPos;

		vec4 vbPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 1, 1);
		vbPos *= 1.f / vbPos.w;
		vec4 wbPos = view_mtx.inverse() * vbPos;

		ray newRay;
		newRay.origin = wfPos;
		newRay.velocity = (wbPos - wfPos);

		return newRay;
	}

	ray CameraControls::WindowsPointToRay(const vec2& vp_pos)
	{
		vec2 ndcPos = vp_pos;

		const auto view_mtx = current_camera->ViewMatrix();
		const auto pers_mtx = current_camera->ProjectionMatrix();

		//-1 to 1 (ndc)
		ndcPos -= vec2(0.5f, 0.5f);
		ndcPos /= 0.5f;

		ndcPos = vec2(ndcPos.x, -ndcPos.y);

		vec4 vfPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 0, 1);
		vfPos *= 1.f / vfPos.w;
		vec4 wfPos = view_mtx.inverse() * vfPos;

		vec4 vbPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 1, 1);
		vbPos *= 1.f / vbPos.w;
		vec4 wbPos = view_mtx.inverse() * vbPos;

		ray newRay;
		newRay.origin = wfPos;
		newRay.velocity = (wbPos - wfPos);

		return newRay;
	}
	vec2 CameraControls::WindowsToNDC(const vec2& pos)
	{
		vec2 ndcPos = pos;
		ndcPos -= vec2(0.5f, 0.5f);
		ndcPos /= 0.5f;

		return vec2(ndcPos.x, -ndcPos.y);
	}
	vec3 CameraControls::NDCToNearPointInWorld(const vec2& pos)
	{
		const auto view_mtx = current_camera->ViewMatrix();
		const auto pers_mtx = current_camera->ProjectionMatrix();

		//-1 to 1 (ndc)
		vec2 ndcPos = pos;

		vec4 vfPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 0, 1);
		vfPos *= 1.f / vfPos.w;
		vec4 wfPos = view_mtx.inverse() * vfPos;

		return wfPos;
	}
	vec3 CameraControls::NDCToFarPointInWorld(const vec2& pos)
	{
		const auto view_mtx = current_camera->ViewMatrix();
		const auto pers_mtx = current_camera->ProjectionMatrix();

		//-1 to 1 (ndc)
		vec2 ndcPos = pos;

		vec4 vbPos = pers_mtx.inverse() * vec4(ndcPos.x, ndcPos.y, 1, 1);
		vbPos *= 1.f / vbPos.w;
		vec4 wbPos = view_mtx.inverse() * vbPos;

		return wbPos;
	}

	vec3 CameraControls::convertMouseCoord(const vec2& screenCoord)
	{
        const real d = screenCoord.x * screenCoord.x + screenCoord.y + screenCoord.y;
        const real radiusSq = _radius * _radius;

		//Check if the point is out of the ball radius
		return  (d > radiusSq) ?
			vec3(screenCoord.x, screenCoord.y, 0) :
			vec3(screenCoord.x, screenCoord.y, sqrt(radiusSq - d));

	}
};
