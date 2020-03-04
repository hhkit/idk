#include "stdafx.h"
#include <phys/ContactSolver.h>

namespace idk
{
	void ContactSolver::Initialize(const CollisionList& collisions)
	{
		
	}

	void ContactSolver::Solve(float dt)
	{
		// for (const auto& [i, j, result] : collision_frame)
			// {
			// 	const auto& lcollider = *i->collider;
			// 	const auto& rcollider = *j->collider;
			// 
			// 	auto lrigidbody = lcollider._rigidbody;
			// 	auto rrigidbody = rcollider._rigidbody;
			// 
			// 	// triggers do not require resolution
			// 	if (lcollider.is_trigger || rcollider.is_trigger)
			// 		continue;
			// 
			// 	struct RigidBodyInfo
			// 	{
			// 		vec3 velocity = {};
			// 		real inv_mass = 0.f;
			// 		RigidBody* ref = nullptr;
			// 	};
			// 
			// 	const auto [lvel, linv_mass, lrb_ptr] =
			//         RigidBodyInfo{ lrigidbody->_pred_tfm[3].xyz - lrigidbody->_prev_pos, lrigidbody->inv_mass, &*lrigidbody };
			//     const auto [rvel, rinv_mass, rrb_ptr] = rrigidbody ?
			//         RigidBodyInfo{ rrigidbody->_pred_tfm[3].xyz - rrigidbody->_prev_pos, rrigidbody->inv_mass, &*rrigidbody } : RigidBodyInfo{};
			// 
			// 	auto rel_v = rvel - lvel; // a is not moving
			// 	auto contact_v = rel_v.dot(result.normal_of_collision); // normal points towards A
			// 
			// 	if (contact_v < +epsilon)
			// 		continue;
			// 
			// 	// determine collision distribution
			// 	auto restitution = (lcollider.bounciness, rcollider.bounciness) * .5f;
			// 	restitution = std::max(restitution - restitution_slop, 0.f);
			// 	IDK_ASSERT(result.penetration_depth > -epsilon);
			// 
			// 	// determine friction disribution
			// 
			// 	{
			// 		const auto sum_inv_mass = linv_mass + rinv_mass;
			// 		const auto collision_impulse_scalar = (1.0f + restitution) * contact_v / sum_inv_mass;
			// 		const auto collision_impulse = damping * collision_impulse_scalar * result.normal_of_collision;
			// 
			// 		const auto penetration = std::max(result.penetration_depth - penetration_min_slop, 0.0f);
			// 		const auto correction_vector = penetration * penetration_max_slop * result.normal_of_collision;
			// 
			// 		const auto tangent = (rel_v - (rel_v.dot(result.normal_of_collision)) * result.normal_of_collision).normalize();
			// 		const auto frictional_impulse_scalar = (1.0f + restitution) * rel_v.dot(tangent) / sum_inv_mass;
			// 		const auto mu = (lcollider.static_friction + rcollider.static_friction) * .5f;
			// 		const auto jtangential = -rel_v.dot(tangent) / sum_inv_mass;
			// 
			// 		const auto frictional_impulse = abs(jtangential) < frictional_impulse_scalar * mu
			// 			? frictional_impulse_scalar * tangent
			// 			: (lcollider.dynamic_friction, rcollider.dynamic_friction) * .5f * frictional_impulse_scalar * tangent;
			// 
			// 		if (lrb_ptr && !lrb_ptr->is_kinematic)
			// 		{
			//             auto& predicted_pos = lrb_ptr->_pred_tfm[3].xyz;
			// 			predicted_pos = predicted_pos + correction_vector;
			// 			const auto new_vel = lvel + (collision_impulse + frictional_impulse) * lrb_ptr->inv_mass;
			//             lrb_ptr->_prev_pos = predicted_pos - new_vel;
			// 		}
			// 
			// 		if (rrb_ptr && !rrb_ptr->is_kinematic)
			// 		{
			//             auto& predicted_pos = rrb_ptr->_pred_tfm[3].xyz;
			//             predicted_pos = predicted_pos - correction_vector;
			// 			const auto new_vel = rvel - (collision_impulse + frictional_impulse) * rrb_ptr->inv_mass;
			//             rrb_ptr->_prev_pos = predicted_pos - new_vel;
			// 		}
			// 	}
	}
}