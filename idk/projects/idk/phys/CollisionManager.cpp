#include "stdafx.h"

#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>

#include <math/matrix_decomposition.h>
#include <math/matrix_transforms.h>

#include <phys/PhysicsSystem.h>
#include <phys/CollisionManager.h>
#include <phys/ContactSolver.h>
#include <phys/Collider.h>
#include <phys/RigidBody.h>
#include <phys/Collision_utils.h>
#include <phys/raycasts/collision_raycast.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <phys/collision_detection/collision_capsule.h>
#include <phys/collision_detection/collision_capsule_box.h>
#include <phys/collision_detection/collision_capsule_sphere.h>
#include <phys/collision_detection/ManagedCollision.h>
#include <phys/collision_contact.h>

#include <gfx/DebugRenderer.h>

#include <scene/SceneManager.h>
#include <scene/Scene.h>

namespace idk
{
	constexpr float baumgarte = .2f;
	constexpr float penetration_slop = 0.05f;
	constexpr float margin = 0.2f;
	constexpr int	collision_threshold = 64;

	void CollisionManager::InitializeNewFrame(span<class RigidBody>, span<class Collider> colliders)
	{
		const auto dt = Core::GetDT().count();
		_dynamic_info.clear();
		for (auto& elem : colliders)
		{
			// bool to_insert = false;
			bool to_insert = false;
			bool to_remove = false;

			// Active/Enabled
			const bool is_active = elem.is_enabled_and_active();
			const bool was_active = elem._active_cache;
			elem._active_cache = is_active;

			// Static/non-static
			const bool was_static = elem._static_cache;
			elem.find_rigidbody();
			const bool is_static = elem._static_cache;

			const bool is_prefab = elem.GetHandle().scene == Scene::prefab;
			// Update tree based on colliders that are active
			if (!is_prefab)
			{
				// deactivated this frame
				if (was_active && !is_active)
				{
					// make sure its static
					if (is_static || was_static)
					{
						to_remove = true;
					}
				}
				// Activated this frame
				else if (is_active && !was_active)
				{
					to_insert = true;
				}
				// Was neither activated nor deactivated
				else if (is_active)
				{
					// Check if the had a rigidbody added to it
					if (was_static && !is_static)
					{
						to_remove = true;

					}
					// Rigidbody removed
					else if (is_static && !was_static)
					{
						to_insert = true;
					}
				}
			}

			if (to_remove)
			{
				if (_static_broadphase.remove(elem.node_id))
				{
					// LOG_TO(LogPool::PHYS, "REMOVED from tick: %u, %u, %u", elem.GetHandle().index, elem.GetHandle().gen, elem.GetHandle().scene);
					elem.node_id = -1;
				}
			}

			if (!is_active || is_prefab)
				continue;

			if (is_static)
			{
				const auto collider_info = std::visit([&elem](const auto& shape) -> ColliderInfo {
					auto pred_shape = shape * elem.GetGameObject()->Transform()->GlobalMatrix();
					return ColliderInfo{ .collider = &elem, .broad_phase = pred_shape.bounds(), .predicted_shape = pred_shape, .layer = elem.GetGameObject()->Layer() };
					}, elem.shape);
				if (to_insert)
				{
					_static_broadphase.insert(elem, collider_info, margin);
				}
				else
				{
					_static_broadphase.update(elem, collider_info, margin);
				}
			}
			else
			{
				// Shape and broadphase are computed at UpdateDynamics
				const auto collider_info = std::visit([&elem]() -> ColliderInfo {	
					return ColliderInfo{ .collider = &elem,.rb = &*elem._rigidbody, .layer = elem.GetGameObject()->Layer() };
				});

				// Reset the global inertia tensor
				collider_info.rb->_global_inertia_tensor = mat3{ scale(vec3{ 0.0f }) };
				// collider_info.rb->_forces_applied = false;
				_dynamic_info.emplace_back(collider_info);
			}
		}
	}

	void CollisionManager::ApplyGravityAndForces(span<class RigidBody> rbs)
	{
		const auto dt = Core::GetDT().count();

		for (auto& rigidbody: rbs)
		{
			const auto tfm = rigidbody.GetGameObject()->Transform();
			rigidbody._global_cache = tfm->GlobalMatrix();
			rigidbody._rotate_cache = tfm->GlobalRotation();

			if (rigidbody.sleeping())
			{
				rigidbody._prev_pos = rigidbody._global_cache[3].xyz;
				rigidbody.linear_velocity = rigidbody.initial_velocity * dt;
				rigidbody.initial_velocity = vec3{};
			}
			else
			{
				// Apply gravity
				if (rigidbody.use_gravity && !rigidbody.is_kinematic)
					rigidbody.AddForce(vec3{ 0, -9.81f * rigidbody.gravity_scale * rigidbody.mass(), 0 });

				// Apply forces
				if (!rigidbody.is_kinematic)
				{
					rigidbody.linear_velocity += (rigidbody.force * rigidbody.inv_mass) * dt;
					rigidbody.linear_velocity *= 1.0f / (1.0f + dt * rigidbody.linear_damping);

					// Compute the angular stuff here too. 
					// body->m_angularVelocity += (body->m_invInertiaWorld * body->m_torque) * m_dt;
					if (!rigidbody.freeze_rotation)
						rigidbody.angular_velocity *= 1.0f / (1.0f + dt * rigidbody.angular_damping);

					// LOG_TO(LogPool::PHYS, "Force: (%f, %f, %f) ||  Velocity: (%f, %f, %f)", 
					//		  rigidbody.force.x, rigidbody.force.y, rigidbody.force.z, rigidbody.linear_velocity.x, rigidbody.linear_velocity.y, rigidbody.linear_velocity.z);
				}
				else
				{
					rigidbody.linear_velocity = (rigidbody._global_cache[3].xyz - rigidbody._prev_pos) / dt;
					rigidbody.angular_velocity = vec3{ 0.0f };
				}

				// Clear all forces
				rigidbody.force = vec3{ 0.0f };
				rigidbody.torque = vec3{ 0.0f };
			}
		}
	}

	void CollisionManager::UpdateDynamics()
	{
		const auto dt = Core::GetDT().count();
		// Only need to update dynamic objects
		for (auto& elem : _dynamic_info)
		{
			std::visit([&elem](const auto& shape) {
				auto pred_shape = shape * elem.rb->_global_cache;
				elem.broad_phase = pred_shape.bounds();
				elem.predicted_shape = pred_shape;

				auto& rb = *elem.rb;
				// No inertia tensor for triggers
				if (elem.collider->is_trigger || rb.freeze_rotation)
					return;
				// Compute the inverse world inertia tensor
				using Shape = decltype(pred_shape);
				
				const mat3 curr_rot = quat_cast<mat3>(rb._rotate_cache);
				const mat3 curr_rot_inv = curr_rot.transpose();
				
				if constexpr (std::is_same_v<Shape, box>)
				{
					constexpr float scalar = 1.0f / 12.0f;

					const vec3 extents = pred_shape.half_extents();
					const vec3 extents_sq = extents * extents;
					
					const float mass = rb.mass();
					const float ex2 = 4.0f * extents.x * extents.x;
					const float ey2 = 4.0f * extents.y * extents.y;
					const float ez2 = 4.0f * extents.z * extents.z;
					const float x = scalar * mass * (ey2 + ez2);
					const float y = scalar * mass * (ex2 + ez2);
					const float z = scalar * mass * (ex2 + ey2);
					
					const mat3 local_tensor{
						vec3{1.0f/ x,   0.0f, 0.0f},
						vec3{0.0f, 1.0f/ y,   0.0f},
						vec3{0.0f, 0.0f, 1.0f/z }
					};
					
					rb._global_inertia_tensor = curr_rot * (rb.inv_mass * local_tensor) * curr_rot_inv;
				}
				else if constexpr (std::is_same_v<Shape, sphere>)
				{
					// 1/mass * (0.4 * r^2) = 1 / (mass * 0.4 * r^2)
					const float diag_val = (1.0f / (0.4f * pred_shape.radius * pred_shape.radius));
					auto gg = vec3{ diag_val };
					const mat3 local_tensor = mat3{ scale(gg) };
					rb._global_inertia_tensor = curr_rot * (rb.inv_mass * local_tensor) * curr_rot_inv;
				}
				else // Only sphere and box has rotational
				{
					rb.freeze_rotation = true;
				}

			}, elem.collider->shape);
		}
	}

	// #pragma optimize("", off)
	void CollisionManager::TestCollisions()
	{
		auto& phys = Core::GetSystem<PhysicsSystem>();
		_info.clear();
		_info.reserve(_dynamic_info.size() * 4);

		// Dynamic vs Dynamic Broadphase
		auto dynamic_end = _dynamic_info.end();
		for (auto i = _dynamic_info.begin(); i != dynamic_end; ++i)
		{
			const auto& lrigidbody = *i->rb;
			const bool lrb_sleeping = lrigidbody.sleeping();
			for (auto j = i + 1; j != dynamic_end; ++j)
			{
				const auto& rrigidbody = *j->rb;

				if (lrb_sleeping && rrigidbody.sleeping())
					continue;
				if (lrigidbody.GetHandle() == rrigidbody.GetHandle())
					continue;
				if (!phys.AreLayersCollidable(i->layer, j->layer))
					continue;
				if (!i->broad_phase.overlaps(j->broad_phase))
					continue;

				_info.emplace_back(ColliderInfoPair{ &*i, &*j });
			}

			if (lrigidbody.sleeping())
				continue;

			// Static vs Dynamic Broadphase
			_static_broadphase.query_collisions(*i, _info);
		}

		// Narrow phase.
		_collisions.clear();
		_collisions.reserve(_info.size());
		constraint_states.clear();
		constraint_states.reserve(_info.size());
		for (auto& pair : _info)
		{
			auto& i = pair.lhs;
			auto& j = pair.rhs;
			const auto collision = CollideShapes(i->predicted_shape, j->predicted_shape);
			if (collision)
			{
				const auto col_val = collision.value();
				ContactConstraintState ccs;
				// Both must not be triggers.
				ccs.solve = !i->collider->is_trigger && !j->collider->is_trigger;
				if (ccs.solve)
				{
					// broadphase center and world center are the same
					ccs.centerA = col_val.centerA;
					ccs.centerB = col_val.centerB;
					ccs.rbA = i->rb;
					ccs.rbB = j->rb;
					ccs.mA = i->rb->inv_mass;
					ccs.mB = j->rb ? j->rb->inv_mass : 0.0f;
					ccs.iA = ccs.rbA->_global_inertia_tensor;
					ccs.iB = ccs.rbB ? ccs.rbB->_global_inertia_tensor : mat3{ vec3{0.0f}, vec3{0.0f}, vec3{0.0f} };
					ccs.restitution = min(i->collider->bounciness, j->collider->bounciness);
					ccs.static_friction = std::sqrt(i->collider->static_friction * j->collider->static_friction);
					ccs.friction = std::sqrt(i->collider->dynamic_friction * j->collider->dynamic_friction);

					ccs.tangentVectors[0] = col_val.tangentVectors[0];
					ccs.tangentVectors[1] = col_val.tangentVectors[1];
					ccs.normal = col_val.normal;

					ccs.contactCount = col_val.contactCount;
					for (int k = 0; k < ccs.contactCount; ++k)
					{
						ContactState* s = ccs.contacts + k;
						const phys::ContactPoint* cp = col_val.contacts + k;

						s->ra = cp->position - ccs.centerA;
						s->rb = cp->position - ccs.centerB;
						s->penetration = cp->penetration;
					}

					constraint_states.emplace_back(ccs);
				}
				_collisions.emplace(CollisionPair{ i->collider->GetHandle(), j->collider->GetHandle() }, col_val);
			}
		}
	}

	void CollisionManager::PreSolve()
	{
		const float dt = Core::GetDT().count();
		constexpr auto invert = [](float a) { return a != 0.0f ? 1.0f / a : 0.0f; };

		for (int i = 0; i < constraint_states.size(); ++i)
		{
			ContactConstraintState& cs = constraint_states[i];

			vec3 vA = cs.rbA->linear_velocity;
			vec3 wA = cs.rbA->angular_velocity;
			vec3 vB = cs.rbB ? cs.rbB->linear_velocity : vec3{ 0.0f };
			vec3 wB = cs.rbB ? cs.rbB->angular_velocity : vec3{ 0.0f };
			// LOG_TO(LogPool::PHYS, "===================== COLLIDE =======================");
			for (int j = 0; j < cs.contactCount; ++j)
			{
				ContactState* c = cs.contacts+ j;

				// Precalculate JM^-1JT for contact and friction constraints
				vec3 raCn = c->ra.cross(cs.normal);
				vec3 rbCn = c->rb.cross(cs.normal);
				float nm = cs.mA + cs.mB;
				float tm[2];
				tm[0] = nm;
				tm[1] = nm;

				nm += raCn.dot(cs.iA * raCn) + rbCn.dot(cs.iB * rbCn);
				c->normalMass = invert(nm);

				for (int k = 0; k < 2; ++k)
				{
					const vec3 raCt = cs.tangentVectors[k].cross(c->ra);
					const vec3 rbCt = cs.tangentVectors[k].cross(c->rb);
					tm[k] += raCt.dot(cs.iA * raCt) + rbCt.dot(cs.iB * rbCt);
					c->tangentMass[k] = invert(tm[k]);
				}
				// LOG_TO(LogPool::PHYS, "Tangent Mass: %f, %f", c->tangentMass[0], c->tangentMass[1]);

				// Precalculate bias factor
				const float tmp = c->penetration + penetration_slop;
				const float tmp2 = min(0.0f, tmp);
				c->bias = -baumgarte * (1.0f / dt) * tmp2;
				// LOG_TO(LogPool::PHYS, "Penetration: %f,		bias: %f", c->penetration, c->bias);
				// LOG_TO(LogPool::PHYS, "Normal: (%f, %f, %f)", cs.normal.x, cs.normal.y, cs.normal.z);
				// Warm start contact
				vec3 P = cs.normal * c->normalImpulse;

				// if (m_enableFriction)
				{
					P += cs.tangentVectors[0] * c->tangentImpulse[0];
					P += cs.tangentVectors[1] * c->tangentImpulse[1];
				}

				vA -= P * cs.mA;
				wA -= cs.iA * c->ra.cross(P);

				vB += P * cs.mB;
				wB += cs.iB * c->rb.cross(P);

				// Add in restitution bias
				float dv = (vB + wB.cross(c->rb) - vA - wA.cross(c->ra)).dot(cs.normal);

				if (dv < -1.0f)
					c->bias += -(cs.restitution) * dv;
			}

			cs.rbA->linear_velocity = vA;
			cs.rbA->angular_velocity = wA;
			if (cs.rbB)
			{
				cs.rbB->linear_velocity = vB;
				cs.rbB->angular_velocity = wB;
			}
		}
	}

	void CollisionManager::Solve()
	{
		for (int iter = 0; iter < 10; ++iter)
		{
			for (int i = 0; i < constraint_states.size(); ++i)
			{
				ContactConstraintState* cs = constraint_states.data() + i;

				vec3 vA = cs->rbA->linear_velocity;
				vec3 wA = cs->rbA->angular_velocity;
				vec3 vB = cs->rbB ? cs->rbB->linear_velocity : vec3{ 0.0f };
				vec3 wB = cs->rbB ? cs->rbB->angular_velocity : vec3{ 0.0f };

				for (int j = 0; j < cs->contactCount; ++j)
				{
					ContactState* c = cs->contacts + j;

					// relative velocity at contact
					vec3 dv = vB + wB.cross(c->rb) - vA - wA.cross(c->ra);

					// Friction
					// if (m_enableFriction)
					{
						for (int k = 0; k < 2; ++k)
						{
							// jt
							float lambda = -dv.dot(cs->tangentVectors[k]) * c->tangentMass[k];

							// Calculate frictional impulse
							float maxLambda = cs->static_friction * c->normalImpulse; 
							if (abs(lambda) < maxLambda)
							{
								// LOG_TO(LogPool::PHYS, "STATIC CHOSEN");
							}
							else
							{
								maxLambda = cs->friction * c->normalImpulse;
								// LOG_TO(LogPool::PHYS, "DYNAMIC CHOSEN");
							} 

							// Clamp frictional impulse
							// float oldStatic = c->tangentStaticImpulse[k];
							// c->tangentStaticImpulse[k] = [&](float min, float max, float a) -> float
							// {
							// 	if (a < min)
							// 		return min;
							// 
							// 	if (a > max)
							// 		return max;
							// 
							// 	return a;
							// }(-maxStatic, maxStatic, oldStatic + lambda);
							// float newStatic = c->tangentStaticImpulse[k] - oldStatic;

							// Clamp frictional impulse
							float oldPT = c->tangentImpulse[k];
							c->tangentImpulse[k] = [&](float min, float max, float a) -> float
							{
								if (a < min)
									return min;

								if (a > max)
									return max;

								return a;
							}(-maxLambda, maxLambda, oldPT + lambda);

							lambda = c->tangentImpulse[k] - oldPT;
							// lambda = abs(lambda) < newStatic ? newStatic : lambda;

							// Apply friction impulse
							vec3 impulse = cs->tangentVectors[k] * lambda;
							vA -= impulse * cs->mA;
							wA -= cs->iA * c->ra.cross(impulse);

							vB += impulse * cs->mB;
							wB += cs->iB * c->rb.cross(impulse);
						}
					}

					// Normal
					{
						dv = vB + wB.cross(c->rb) - vA - wA.cross(c->ra);

						// Normal impulse
						float vn = dv.dot(cs->normal);

						// Factor in positional bias to calculate impulse scalar j
						float lambda = c->normalMass * (-vn + c->bias);

						// Clamp impulse
						float tempPN = c->normalImpulse;
						c->normalImpulse = max(tempPN + lambda, 0.0f);
						lambda = c->normalImpulse - tempPN;

						// Apply impulse
						vec3 impulse = cs->normal * lambda;
						vA -= impulse * cs->mA;
						wA -= cs->iA * c->ra.cross(impulse);

						vB += impulse * cs->mB;
						wB += cs->iB * c->rb.cross(impulse);
					}
				}

				cs->rbA->linear_velocity = vA;
				cs->rbA->angular_velocity = wA;
				if (cs->rbB)
				{
					cs->rbB->linear_velocity = vB;
					cs->rbB->angular_velocity = wB;
				}
			}
			if (constraint_states.size() > 0)
			{
				const auto vel = constraint_states[0].rbA->linear_velocity;
				//LOG_TO(LogPool::PHYS, "Solved Velocity: (%f, %f, %f)", vel.x, vel.y, vel.z);
			}
			
		}
	}

	void CollisionManager::Finalize(span<class RigidBody> rbs)
	{
		const float dt = Core::GetDT().count();
		for (auto& rigidbody : rbs)
		{
			if (!rigidbody.is_kinematic)
			{
				// Velocity
				vec3 translate = rigidbody._global_cache[3].xyz;
				if (rigidbody.linear_velocity.dot(rigidbody.linear_velocity) > 0.0001f)
				{
					translate = translate + rigidbody.linear_velocity * dt;
				}
				else
				{
					rigidbody.linear_velocity = vec3{ 0.0f };
				}
				// Rotational
				// Only do if there is angular velocity and rotational is not frozen
				if (!rigidbody.freeze_rotation && rigidbody.angular_velocity.dot(rigidbody.angular_velocity) > 0.0001f)
				{
					// Scale
					const vec3 s = [](const mat4& mat)
					{
						auto det = mat.determinant();
						return vec3(det < -epsilon ? -mat[0].length() : mat[0].length(), mat[1].length(), mat[2].length());
					}(rigidbody._global_cache);

					// Rotation
					quat q = rigidbody._rotate_cache;
					q = q.integrate(rigidbody.angular_velocity * dt);
					const mat4 r = quat_cast<mat4>(q);

					rigidbody._global_cache = r * scale(s);
				}
				else
				{
					rigidbody.angular_velocity = vec3{ 0.0f };
				}

				rigidbody._global_cache[3].xyz = translate;
				rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._global_cache);
			}
			else
			{
				rigidbody.linear_velocity = (rigidbody._global_cache[3].xyz - rigidbody._prev_pos) / dt;
				rigidbody._prev_pos = rigidbody._global_cache[3].xyz;
				rigidbody.angular_velocity = vec3{ 0.0f };
			}
			    
			rigidbody.sleep_next_frame = false;
			// rigidbody._done_this_frame = true;
		}
	}

	void CollisionManager::Init()
	{
		_static_broadphase.preallocate_nodes(2500); // Avg ~1030 static objects -> means (2 * 1030 - 1) total nodes in b-tree
		
		GameState::GetGameState().OnObjectCreate<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider)
				return;

			auto& col = *collider;
			col._static_cache = col.is_enabled_and_active();
			if (!col._static_cache || col.GetHandle().scene == Scene::prefab)
				return;

			col.find_rigidbody();
			if (col._static_cache)
			{
				_static_broadphase.insert(col, col.bounds(), 0.2f);
				// LOG_TO(LogPool::PHYS, "INSERTED from object create: %u, %u, %u", collider.index, collider.gen, collider.scene);
			}
		};

		GameState::GetGameState().OnObjectDestroy<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider)
				return;

			auto& col = *collider;
			if (!col._static_cache)
				return;

			if (_static_broadphase.remove(col.node_id))
			{
				// LOG_TO(LogPool::PHYS, "REMOVED from object create: %u, %u, %u", collider.index, collider.gen, collider.scene);
				col.node_id = -1;
				col._active_cache = false;
			}
		};
	}

	void CollisionManager::Reset()
	{
		_dynamic_info.clear();
		_collisions.clear();
		constraint_states.clear();
		_info.clear();
		_static_broadphase.clear();
	}

	void CollisionManager::DebugDrawContactPoints(float dt)
	{
		auto& dbg = Core::GetSystem<DebugRenderer>();
		for (auto col : constraint_states)
		{
			for (int i = 0; i < col.contactCount; ++i)
			{
				const auto& c = col.contacts[i];
				const auto position = col.centerA + c.ra;
				dbg.Draw(position, color{ 0,1,0,1 }, seconds{ dt });
				dbg.Draw(ray{ position, col.normal * 0.4f }, color{ 0,1,0,1 }, seconds{ dt });
				dbg.Draw(ray{ position, col.tangentVectors[0] * 0.4f }, color{ 0,0,1,1 }, seconds{ dt });
				dbg.Draw(ray{ position, col.tangentVectors[1] * 0.4f }, color{ 0,0,1,1 }, seconds{ dt });
			}
			// LOG_TO(LogPool::PHYS, "Contact Count: %d", col.contactCount);
		}
	}

	void CollisionManager::DebugDrawColliders(span<class Collider> colliders, float dt)
	{
		constexpr auto debug_draw = [&](const CollidableShapes& pred_shape, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
				{
					Core::GetSystem<DebugRenderer>().Draw(shape, c, dur);
				}, pred_shape);
		};

		_static_broadphase.debug_draw();
		for (const auto& elem : _dynamic_info)
			debug_draw(elem.predicted_shape, elem.collider->is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 }, seconds{ dt });
		for (auto& elem : colliders)
		{
			if (!elem._active_cache)
			{
				std::visit([&](const auto& shape)
					{
						debug_draw(calc_shape(shape, elem), color{ 0.5 }, seconds{ dt });
					}, elem.shape);
			}
		}
	}

	void CollisionManager::BuildStaticTree()
	{
		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();
		BuildStaticTree(colliders);
	}

	void CollisionManager::BuildStaticTree(span<Collider> colliders)
	{
		_static_broadphase.clear();
		for (auto& col : colliders)
		{
			col._active_cache = col.is_enabled_and_active();
			if (!col._active_cache || col.GetHandle().scene == Scene::prefab)
				return;

			col.find_rigidbody();
			if (col._static_cache)
			{
				_static_broadphase.insert(col, col.bounds(), 0.2f);
			}
		}
	}
	phys::col_result CollisionManager::CollideShapes(const CollidableShapes& lhs, const CollidableShapes& rhs)
	{
		constexpr auto Collide = [](const auto& lshape, const auto& rshape) -> phys::col_result
	    {
		    using LShape = std::decay_t<decltype(lshape)>;
		    using RShape = std::decay_t<decltype(rshape)>;

		    if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, box>)
			    return phys::collide_box_box_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, box>)
			    return -phys::collide_box_sphere_discrete(
				    rshape, lshape);
		    else
		    if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, sphere>)
			    return phys::collide_sphere_sphere_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, sphere>)
			    return phys::collide_box_sphere_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, sphere>)
			    return phys::collide_capsule_sphere_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, capsule>)
			    return phys::collide_capsule_sphere_discrete(
				    rshape, lshape);
		    else
		    if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, box>)
			    return phys::collide_capsule_box_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, capsule>)
			    return phys::collide_capsule_box_discrete(
				    rshape, lshape);
		    else
		    if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, capsule>)
			    return phys::collide_capsule_capsule_discrete(
				    lshape, rshape);
		    else
			    return phys::col_failure{};
	    };

		return std::visit(Collide, lhs, rhs);
	}
}


