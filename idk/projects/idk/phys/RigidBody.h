#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class RigidBody
		: public Component<RigidBody>
	{
	public:
		vec3 initial_velocity {};
		real drag             { 0.f };

		real inv_mass         { 1.f };
		vec3 center_of_mass   {};

		vec3 axis_of_angular_velocity { 1, 0, 0 };
		rad  angular_velocity {};

		bool use_gravity  { true  };
		bool is_kinematic { false };

		// sleeping
		real sleep_threshold  { +epsilon };
		bool sleep_next_frame { true     };

		vec3 position() const; 
		void position(const vec3& new_pos); //teleport

		real mass() const;
		void mass(real);
		vec3 velocity() const;
		void velocity(const vec3& new_vel);
		bool sleeping() const;
		void sleeping(bool new_bool);

		vec3 AddForce(const vec3& newtons);
		void TeleportBy(const vec3& translation);

		const mat4& PredictedTransform() const;
	private:
		mat4 _predicted_tfm;
		vec3 _accum_accel{};
		vec3 _prev_pos;
		bool _sleeping = false;

		friend class PhysicsSystem;
		friend class Collider;
	};
}