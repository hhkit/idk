#pragma once

namespace idk {
	struct Ray
	{
		vec3 direction{};
		vec3 origin{};
	};
	class CameraControls {
	public:
		CameraControls() = default;
		CameraControls(Handle<Camera>);
		~CameraControls() = default;
		Ray			ViewportPointToRay(const vec3& pos);
		Ray			ViewportPointToRay(const vec2& pos);

		//ArcBall rotation
		void		RotateCamera(const vec2& screenpos);
		void		StopRotatingCamera();
		void		StartRotatingCamera();

		vec3		currentTarget() const;
		void		SetTarget(Handle<Transform>);
		void        SetCurrentCamera(Handle<Camera>);
		void		FocusOnObj(Handle<GameObject>);

		vec3		convertMouseCoord(const vec2&);

		void		LookAt();

		void		Focus();
		
		Handle<Transform> selected_target{};
		Handle<Camera>    current_camera{};
	private:

		vec3 _target{ 0.0f,0.0f,0.0f };
		bool _focusing{ false };
		bool _arcBallRotating{ false };
		bool _rotating{ false };
		vec2 _oldScreenPos{};

		real _alpha{};
		real _beta{};

		vec3 initialVector{};
		vec3 currVector{};

		real _radius{};
	};
};