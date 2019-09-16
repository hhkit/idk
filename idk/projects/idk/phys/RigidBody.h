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

		real inv_mass       = 1.f;
		vec3 center_of_mass = vec3{};

		vec3 axis_of_angular_velocity = vec3{ 1, 0, 0 };
		rad  angular_velocity = rad{};

		bool use_gravity  = true;
		bool is_kinematic = false;
		real restitution  = 1.f;

		// sleeping
		real sleep_threshold  = +epsilon;
		bool sleeping         = false; 
		bool sleep_next_frame = true;

		real mass() const;
		void mass(real);

		vec3 AddForce(const vec3& newtons);

		const mat4& PredictedTransform() const;
	private:
		mat4 _predicted_tfm;
		vec3 _prev_accel;

		friend class PhysicsSystem;
	};
}