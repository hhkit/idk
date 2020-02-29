#pragma once
#include <idk.h>
#include <core/Component.h>

namespace idk
{
	class RigidBody
		: public Component<RigidBody>
	{
	public:
		vec3 initial_velocity { 0.f };
		real drag             { 0.f };

		real inv_mass         { 1.f };
		real inv_inertia      { 1.f };
		vec3 center_of_mass   {};

		vec3 force{ 0.0f };
		vec3 linear_velocity{ 0.0f };
		float linear_damping{ 1.0f };

		vec3 torque{ 0.0f };
		vec3 angular_velocity{ 0.0f };
		float angular_damping{ 0.1f };

		bool use_gravity  { true  };
		float gravity_scale{ 1.0f };
		bool is_kinematic { false };
		bool freeze_rotation{ true };

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
		vec3 AddTorque(const vec3& t);
		void TeleportBy(const vec3& translation);

	private:
		// vec3 _accum_accel{};
		vec3 _prev_pos;
		bool _sleeping = false;
		bool _forces_applied = false;
		bool _done_this_frame = false;
        mat4 _global_cache{};
		quat _rotate_cache{ };
		mat3 _global_inertia_tensor{};
		

		friend class PhysicsSystem;
		friend class CollisionManager;
		friend class Collider;
	};
}