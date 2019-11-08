#pragma once

#include "phys/raycasts/collision_raycast.h"

namespace idk {
	class CameraControls {
	public:
		CameraControls() = default;
		CameraControls(Handle<Camera>);
		~CameraControls() = default;
		ray			WindowsPointToRay(const vec3& pos);
		ray			WindowsPointToRay(const vec2& pos);
		vec2		WindowsToNDC(const vec2& pos);
		vec3		NDCToNearPointInWorld(const vec2& pos);
		vec3		NDCToFarPointInWorld(const vec2& pos);

		//Rotation
		void		RotateCamera(const vec2& screenpos);
		void		StopRotatingCamera();
		void		StartRotatingCamera(const vec2& screenpos);

		//Panning
		void		StartPanningCamera(const vec2& screenpos);
		void		PanCamera(const vec2& screenpos);
		void		StopPanningCamera();

		vec3		currentTarget() const;
		void		SetTarget(Handle<Transform>);
		void        SetCurrentCamera(Handle<Camera>);
		void		FocusOnObj(Handle<GameObject>);

		vec3		convertMouseCoord(const vec2&);

		void		LookAt();

		void		Focus();
		
		Handle<Transform> selected_target{};
		Handle<Camera>    current_camera{};

		bool _rotating{ false };
		bool _panning{ false };
	private:

		vec3 _target{ 0.0f,0.0f,0.0f };
		bool _focusing{ false };
		bool _arcBallRotating{ false };
		vec2 _oldScreenPos{};

		real _alpha{};
		real _beta{};
		real camSpd{1.f};
		real panSpd{60.f};

		vec3 initialVector{};
		vec3 currVector{};

		real _radius{};
	};
};