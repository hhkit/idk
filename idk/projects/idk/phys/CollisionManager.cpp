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

#include <scene/Scene.h>

namespace idk
{
	constexpr float baumgarte = 0.3f;
	constexpr float restitution_slop = 0.2f;
	constexpr float penetration_slop = 0.07f;
	constexpr float penetration_max_slop = 0.5f;
	constexpr float margin = 0.2f;
	constexpr int	collision_threshold = 64;

	void CollisionManager::UpdatePairs(span<class RigidBody> rbs, span<class Collider> colliders)
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
				// static_info.emplace_back(collider_info);
				if (to_insert)
				{
					// inserted_within_tick.emplace(elem.GetHandle());
					// LOG_TO(LogPool::PHYS, "INSERTED from TICK: %u, %u, %u", elem.GetHandle().index, elem.GetHandle().gen, elem.GetHandle().scene);
					_static_broadphase.insert(elem, collider_info, margin);
				}
				else
					_static_broadphase.update(elem, collider_info, margin);
			}
			else
			{
				// ColliderInfo d_info{ .collider = &elem, .rb = &*elem._rigidbody, .layer = elem.GetGameObject()->Layer() };
				// auto pred_tfm = d_info.rb->_global_cache;
				// pred_tfm[3].xyz = vec3{ 0.0f };
				// pred_tfm = quat_cast<mat4>(d_info.rb->_pred_rotate) * pred_tfm;
				// pred_tfm[3].xyz = d_info.rb->_pred_translation;
				// 
				// d_info.predicted_shape = std::visit([&](const auto& shape)->CollidableShapes { return shape * pred_tfm; }, d_info.collider->shape);
				// d_info.broad_phase = std::visit([&](const auto& shape) { return (shape * pred_tfm).bounds(); }, d_info.collider->shape);
				// auto vel = d_info.collider->_rigidbody->velocity() * dt;
				// d_info.broad_phase.grow(vel);
				// d_info.broad_phase.grow(-vel);
				const auto collider_info = std::visit([&elem](const auto& shape) -> ColliderInfo {
					auto pred_shape = shape * elem.GetGameObject()->Transform()->GlobalMatrix();
					return ColliderInfo{ .collider = &elem,.rb = &*elem._rigidbody, .broad_phase = pred_shape.bounds(), .predicted_shape = pred_shape, .layer = elem.GetGameObject()->Layer() };
					}, elem.shape);
				// Compute the world inertia tensor
				
				_dynamic_info.emplace_back(collider_info);
			}
		}
	}
#pragma optimize("", off)
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
			_static_broadphase.query_collisions(*i, _info);
		}

		// Static vs Dynamic Broadphase
		// for (const auto& i : _dynamic_info)
		// {
		// 	const auto& lrigidbody = *i.rb;
		// 
		// 	if (lrigidbody.sleeping())
		// 		continue;
		// 	_static_broadphase.query_collisions(i, _info);
		// }

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
					const auto r = quat_cast<mat3>(i->rb->_rotate_cache);
					// broadphase center and world center are the same
					ccs.centerA = col_val.centerA;
					ccs.centerB = col_val.centerB;
					ccs.rbA = i->rb;
					ccs.rbB = j->rb;
					ccs.mA = i->rb->inv_mass;
					ccs.mB = j->rb ? j->rb->inv_mass : 0.0f;
					ccs.iA = r * mat3{ scale(vec3{6.0f}) }// mat3{ scale(vec3{1.0f/ (0.4f * 0.5f * 0.5f)}) } 
					*r.transpose();
					ccs.iB = r * mat3{ scale(vec3{0.0f}) }// mat3{ scale(vec3{1.0f/ (0.4f * 0.5f * 0.5f)}) } 
					*r.transpose();
					ccs.restitution = max(i->collider->bounciness, j->collider->bounciness);
					ccs.friction = std::sqrt(i->collider->static_friction * j->collider->static_friction);

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

				// Precalculate bias factor
				const float tmp = c->penetration + penetration_slop;
				const float tmp2 = min(0.0f, tmp);
				c->bias = -baumgarte * (1.0f / dt) * tmp2;
				LOG_TO(LogPool::PHYS, "Penetration: %f,		bias: %f", c->penetration, c->bias);
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
						float lambda = -dv.dot(cs->tangentVectors[k]) * c->tangentMass[k];

						// Calculate frictional impulse
						float maxLambda = cs->friction * c->normalImpulse;

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
			LOG_TO(LogPool::PHYS, "Solve Velocity: (%f, %f, %f)", vA.x, vA.y, vA.z);
		}
	}

	void CollisionManager::Finalize()
	{
		const float dt = Core::GetDT().count();
		for (auto& elem : _dynamic_info)
		{
			auto& rigidbody = *elem.collider->_rigidbody;
			if (!rigidbody.is_kinematic)
			{
				// Translate
				const vec3 t = rigidbody._global_cache[3].xyz + rigidbody.linear_velocity * dt;
				// Only do if there is angular velocity
				if (rigidbody.angular_velocity.dot(rigidbody.angular_velocity) > 0.0001f)
				{
					rigidbody.angular_velocity = vec3{ 0.0f };
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

				rigidbody._global_cache[3].xyz = t;
				rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._global_cache);
			}
			else
			{
				rigidbody.linear_velocity = vec3{ 0.0f };
				rigidbody.angular_velocity = vec3{ 0.0f };
			}
			    
			rigidbody.sleep_next_frame = false;
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
		for (auto col : _collisions)
		{
			for (int i = 0; i < col.second.contactCount; ++i)
			{
				const auto& c = col.second.contacts[i];
				dbg.Draw(c.position, color{ 0,1,0,1 }, seconds{ 0.5f });
				dbg.Draw(ray{ c.position, col.second.normal * 0.4f }, color{ 0,1,0,1 }, seconds{ 0.5 });
				dbg.Draw(ray{ c.position, col.second.tangentVectors[0] * 0.4f }, color{ 0,0,1,1 }, seconds{ 0.5 });
				dbg.Draw(ray{ c.position, col.second.tangentVectors[1] * 0.4f }, color{ 0,0,1,1 }, seconds{ 0.5 });
			}
			LOG_TO(LogPool::PHYS, "Contact Count: %d", col.second.contactCount);
		}
	}

	void CollisionManager::DebugDrawColliders(span<class Collider> colliders, float dt)
	{
		constexpr auto debug_draw = [&](const CollidableShapes& pred_shape, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT() + seconds{ 0.02f })
		{
			std::visit([&](const auto& shape)
				{
					Core::GetSystem<DebugRenderer>().Draw(shape, c, dur);
				}, pred_shape);
		};

		_static_broadphase.debug_draw();
		for (const auto& elem : _dynamic_info)
			debug_draw(elem.predicted_shape, elem.collider->is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 }, seconds{ 0.5f });
		for (auto& elem : colliders)
		{
			if (!elem._active_cache)
			{
				std::visit([&](const auto& shape)
					{
						debug_draw(calc_shape(shape, elem), color{ 0.5 });
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


