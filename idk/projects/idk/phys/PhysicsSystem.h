#pragma once
#include <compare>
#include <idk.h>
#include <core/ConfigurableSystem.h>
#include <phys/collision_result.h>
#include <phys/raycasts/collision_raycast.h>
#include <common/LayerManager.h>

namespace idk
{
	struct RaycastHit
	{
		Handle<Collider> collider;
		phys::raycast_success raycast_succ;
	};

    struct PhysicsConfig
    {
        std::array<Layer::LayerMask, LayerManager::num_layers> matrix = { 0xFFFFFFFF };
    };

	class PhysicsSystem
		: public ConfigurableSystem<PhysicsConfig>
	{
	public:
		void PhysicsTick            (span <class RigidBody> rbs, span<class Collider> colliders, span<class Transform>);
		void FirePhysicsEvents();
		void DebugDrawColliders     (span<class Collider> colliders);
		void Reset();

		vector<RaycastHit> Raycast(const ray& r, int layer_mask, bool hit_triggers = false);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList, vector<phys::raycast_result>& ray_resultList);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList);
	private:
		struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };
		struct pair_hasher   { size_t operator()(const CollisionPair&) const; };

		using CollisionList = hash_table<CollisionPair, phys::col_success, pair_hasher>;
		CollisionList collisions;
		CollisionList previous_collisions;
		void Init() override;
		void Shutdown() override;
        void ApplyConfig(Config&) override {}
	};
}