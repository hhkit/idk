#pragma once
#include <compare>
#include <idk.h>
#include <core/ConfigurableSystem.h>
#include <phys/collision_result.h>
#include <phys/raycasts/collision_raycast.h>
#include <common/LayerManager.h>
#include <phys/AAbbTree.h>

namespace idk
{
	struct RaycastHit
	{
		Handle<Collider> collider;
		phys::raycast_success raycast_succ;
	};

    struct PhysicsConfig
    {
        std::array<LayerMask, LayerManager::num_layers> matrix;
		size_t batch_size = 64;
		PhysicsConfig() { matrix.fill(LayerMask{ 0xFFFFFFFF }); }
        PhysicsConfig(const PhysicsConfig&) = default;
        PhysicsConfig(PhysicsConfig&&) = default;
        PhysicsConfig& operator=(const PhysicsConfig&) = default;
        PhysicsConfig& operator=(PhysicsConfig&&) = default;
    };

	class PhysicsSystem
		: public ConfigurableSystem<PhysicsConfig>
	{
	public:
		void PhysicsTick            (span <class RigidBody> rbs, span<class Collider> colliders, span<class Transform>);
		void FirePhysicsEvents();
		void DrawCollider           (const Collider& collider) const;
		void DebugDrawColliders     (span<Collider> colliders);
		void Reset();

		vector<RaycastHit> Raycast(const ray& r, LayerMask layer_mask, bool hit_triggers = false);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList, vector<phys::raycast_result>& ray_resultList);
		bool RayCastAllObj			(const ray& r, vector<Handle<GameObject>>& collidedList);

        bool AreLayersCollidable(LayerManager::layer_t a, LayerManager::layer_t b) const;
		void BuildStaticTree();
		void BuildStaticTree(span<Collider> colliders);
		void ClearStaticTree();

        bool debug_draw_colliders = false;

	private:
		struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };
		struct pair_hasher   { size_t operator()(const CollisionPair&) const; };

		using CollisionList = hash_table<CollisionPair, phys::col_success, pair_hasher>;
		CollisionList collisions;
		CollisionList previous_collisions;
		AabbTree static_tree;
		bool _rebuild_tree = false;

		void Init() override;
		void Shutdown() override;
        void ApplyConfig(Config&) override {}
		
	};
}