#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <util/pool.h>

namespace idk
{
	template<typename T>
	class ObjectPool
	{
	public:
		using Handle = Handle<T>;
		using index_t = GenericHandle::index_t;
		using uses_t  = GenericHandle::uses_t;
		using scene_t = GenericHandle::scene_t;
		
		ObjectPool();
		~ObjectPool();

		// iterators
		span<T> GetSpan();

		// accessors
		bool   Validate(const Handle&);
		T*     Get(const Handle&);

		// modifiers
		Handle Create(scene_t scene_id);
		Handle Create(const Handle&);
		bool   Destroy(const Handle&);

		bool ActivateScene(scene_t scene_id, size_t reserve = 8192);
		bool DeactivateScene(scene_t scene_id);

	private:
		static constexpr index_t invalid = index_t{ 0xFFFFFFFF };
		struct Inflect
		{
			index_t index = invalid;
			uses_t  uses  = 0;
		};

		struct Map
		{
			vector<Inflect> slots;
			unsigned first_free = 0;
			void shift() { while (first_free != slots.size() && slots[first_free].index != invalid) ++first_free; }
			void grow() { slots.resize(slots.size() * 3 / 2); }
		};

		array<Map, MaxScene> _scenes;
		pool<T> _pool;
	};
}

#include "ObjectPool.inl"