#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class RigidBody
		: public Component<RigidBody>
	{
	public:
		vec3 velocity = vec3{};
		vec3 accel    = vec3{};
		real drag     = 0.f;

		real mass           = 1.f;
		vec3 center_of_mass = vec3{};

		vec3 axis_of_angular_velocity = vec3{ 1, 0, 0 };
		rad  angular_velocity = rad{};

		bool use_gravity  = true;
		bool is_kinematic = true;

		// sleeping
		real sleep_threshold  = +epsilon;
		bool sleeping         = false; 
		bool sleep_next_frame = false;

		vec3 AddForce(const vec3& newtons);
	};
}