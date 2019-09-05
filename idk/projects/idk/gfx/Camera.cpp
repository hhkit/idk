#include "stdafx.h"
#include "Camera.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <math/matrix_transforms.h>
#include <math/matrix_decomposition.h>

//Test
#include <app/Application.h>

namespace idk
{
	void Camera::LookAt(vec3 target_point, vec3 up)
	{
		auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), target_point, up)));

		_target = target_point;

	}

	void Camera::LookAt()
	{
		auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), _target, tfm->Up())));

	}

	void Camera::Focus()
	{
		auto tfm = GetGameObject()->Transform();

		tfm->GlobalRotation(decompose_rotation_matrix(look_at(tfm->GlobalPosition(), _target, tfm->Up())));

		_focusing = true;
	}

	vec3 Camera::currentPosition() const
	{
		auto tfm = GetGameObject()->Transform();

		return tfm->GlobalPosition();
	}

	vec3 Camera::currentTarget() const
	{
		return _target;
	}

	vec3 Camera::currentDirection() const
	{
		auto tfm = GetGameObject()->Transform();
		//return _direction;
		return GetGameObject()->Transform()->Forward().normalize();

	}

	Ray Camera::ViewportPointToRay(const vec3& vp_pos)
	{
		//Current 
		//auto vec = GetGameObject()->Transform()->GlobalPosition();

		//Viewport(0,1) to screen space (NDC)(-1,1) first 
		vec2 screen_pos = { (2.f * vp_pos.x - 1.f) * aspect, 1.f - 2.f * vp_pos.y };


		//screen space back to 4d clip space 
		vec4 clip_pos = { screen_pos.x,screen_pos.y, -1.f,1.f };


		//Inversing pos from clip space to view space on the near plane
		vec4 viewpoint_pos = ProjectionMatrix().inverse() * clip_pos;

		viewpoint_pos.y = -1.f;
		viewpoint_pos.z = 0.f;

		//Now getting the world position of the point
		vec3 world_pos = (ViewMatrix().inverse() * viewpoint_pos).xyz;
		world_pos = world_pos.normalize();


		//References for this method (http://antongerdelan.net/opengl/raycasting.html)


		return Ray{ currentDirection(),world_pos };
	}

	Ray Camera::ViewportPointToRay(const vec2& vp_pos)
	{
		//Current 
		//auto vec = GetGameObject()->Transform()->GlobalPosition();

		//Viewport(0,1) to screen space (NDC)(-1,1) first 
		vec2 screen_pos = { (2.f * vp_pos.x - 1.f) * aspect, 1.f - 2.f * vp_pos.y };


		//screen space back to 4d clip space 
		vec4 clip_pos = { screen_pos.x,screen_pos.y, -1.f,1.f };


		//Inversing pos from clip space to view space on the near plane
		vec4 viewpoint_pos = ProjectionMatrix().inverse() * clip_pos;

		viewpoint_pos.y = -1.f;
		viewpoint_pos.z = 0.f;

		//Now getting the world position of the point
		vec3 world_pos = (ViewMatrix().inverse() * viewpoint_pos).xyz;
		world_pos = world_pos.normalize();


		//References for this method (http://antongerdelan.net/opengl/raycasting.html)


		return { currentDirection(),world_pos };
	}

	void Camera::RotateArcBallCamera(const vec2& screenpos)
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

			vec3 rotationAxis = currVector.cross(initialVector).normalize();

			real rotAngle = std::acos(currVector.dot(initialVector));

			auto tfm = GetGameObject()->Transform();

			tfm->rotation = (quat{ rotationAxis, deg{rotAngle} } *tfm->rotation).normalize();
			*/


			
			_alpha = screenpos.x - _oldScreenPos.x * 90.f;
			_beta = screenpos.y - _oldScreenPos.y * 90.f;

			float angle_A = _alpha / 180.f * pi;
			float angle_B = _beta / 180.f * pi;

			float maxDeg = 89.f / 180.f * pi;

			mat3 rotMatrix = rotate(vec3(0, 1, 0), rad(angle_A));
			mat3 rotMatrix2 = rotate(vec3(0, 0, 1), rad(angle_B));

			mat4 matrix{ rotMatrix * rotMatrix2 };

			auto tfm = GetGameObject()->Transform();

			Ray result = ViewportPointToRay(screenpos);

			_target = result.origin + result.direction;

			tfm->GlobalRotation(decompose_rotation_matrix(matrix));

			_oldScreenPos = screenpos;
			
		}
	}


	void Camera::StopRotatingArcBallCamera()
	{
		_rotating = false;
		_arcBallRotating = false;
	}

	void Camera::StartRotatingArcballCamera()
	{
		_rotating = true;
	}

	void Camera::SetTarget(const vec3& pos)
	{
		_target = pos;
	}

	mat4 Camera::ViewMatrix() const
	{
		auto mat = GetGameObject()->Transform()->GlobalMatrix();
		auto tfm = GetGameObject()->Transform();
		auto retval = orthonormalize(mat);
		retval[3] = mat[3];

		vec3 upvector = tfm->Up();
		vec3 rightvector = tfm->Right();
		vec3 forwardvector = tfm->Forward();

		mat4 matrix;

		//matrix[0] = vec4(rightvector,0);
		//matrix[1] = vec4(upvector,0);
		//matrix[2] = vec4(forwardvector, 0);
		//matrix[3] = vec4(tfm->GlobalPosition(),1);

		//auto retval1 = orthonormalize(matrix);

		//auto retval1 = matrix.transpose();

		return retval;
	}

	mat4 Camera::ProjectionMatrix() const
	{
		return is_orthographic
			? ortho(-orthographic_size, +orthographic_size, -orthographic_size * aspect, +orthographic_size * aspect, near_plane, far_plane)
			: perspective(field_of_view, aspect, near_plane, far_plane);
	}
	CameraData Camera::GenerateCameraData() const
	{
		return CameraData{
			0xFFFFFFF,
			ViewMatrix(),
			ProjectionMatrix(),
			render_target
		};
	}
	vec3 Camera::convertMouseCoord(const vec2& screenCoord)
	{
		real d = screenCoord.x * screenCoord.x + screenCoord.y + screenCoord.y;
		
		real radiusSq = _radius * _radius;

		//Check if the point is out of the ball radius
		return  (d > radiusSq) ?
			 vec3(screenCoord.x, screenCoord.y, 0):
			 vec3(screenCoord.x, screenCoord.y, sqrt(radiusSq - d));
		
	}
}
