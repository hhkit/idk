#include "ObjectPool.h"
#pragma once
namespace idk
{
	template <typename T>
	ObjectPool<T>::ObjectPool()
	{
	}

	template<typename T>
	inline ObjectPool<T>::~ObjectPool()
	{
	}
	template<typename T>
	inline span<T> ObjectPool<T>::GetSpan()
	{
		return _pool.span();
	}

	template<typename T>
	inline bool ObjectPool<T>::Validate(const Handle& handle)
	{
		if (handle.id == 0)
			return false;

		if (handle.type != Handle::type_id)
			return false;

		auto& scene = _scenes[handle.scene];
		if (handle.index >= scene.slots.size())
			return false;

		auto& inflect = scene.slots[handle.index];

		return inflect.index != invalid && inflect.gen == handle.gen;
	}
	template<typename T>
	inline T* ObjectPool<T>::Get(const Handle& handle)
	{
		if (Validate(handle) == false)
			return nullptr;

		auto& scene = _scenes[handle.scene];
		return &_pool[scene.slots[handle.index].index];
	}

	template<typename T>
	typename ObjectPool<T>::Handle ObjectPool<T>::Create(scene_t scene_id)
	{
		auto& scene = _scenes[scene_id];
		if (scene.slots.empty())
			return {};

		if (scene.slots.size() == scene.first_free)
			scene.grow();

		// generate handle
		auto& slot = scene.slots[scene.first_free];
		auto handle = Handle{ scene.first_free, ++slot.gen, scene_id };
		slot.index = s_cast<index_t>(_pool.emplace_back());
		scene.shift();
		
		return _pool[slot.index].handle = handle;
	}
	
	template<typename T>
	typename ObjectPool<T>::Handle ObjectPool<T>::Create(const Handle& handle)
	{
		if (handle.id == 0)
			return handle;

		auto& scene = _scenes[handle.scene];
		if (scene.slots.empty())
			return Handle{};

		while (scene.slots.size() < handle.index)
			scene.grow();

		auto& slot = scene.slots[handle.index];

		if (slot.index != invalid)
			return Handle{};

		slot.index = static_cast<index_t>(_pool.emplace_back());

		if (handle.index < scene.first_free)
			scene.first_free = handle.index;

		if (handle.index == scene.first_free)
			scene.shift();

		return  _pool[slot.index].handle = handle;
	}

	template<typename T>
	inline bool ObjectPool<T>::Destroy(const Handle& handle)
	{
		if (Validate(handle) == false)
			return false;

		auto& scene = _scenes[handle.scene];

		auto& pool_end = _pool.back();
		auto  end_handle = pool_end.handle;
		auto& end_slot = _scenes[end_handle.scene].slots[end_handle.index];
		auto& destroy_slot = scene.slots[handle.index];

		auto& destroyme = _pool[destroy_slot.index];

		// prepare for destruction
		std::swap(destroyme, pool_end);
		_pool.pop_back();

		// cleanup
		std::swap(end_slot.index, destroy_slot.index);
		destroy_slot.index = invalid;

		if (handle.index < scene.first_free)
			scene.first_free = handle.index;

		return true;
	}
	
	template<typename T>
	bool ObjectPool<T>::ActivateScene(scene_t scene_id, size_t reserve)
	{
		auto& scene = _scenes[scene_id];
		if (scene.slots.size())
			return false;

		scene.slots.resize(reserve);
		scene.first_free = 0;
		return true;
	}

	template<typename T>
	inline bool ObjectPool<T>::DeactivateScene(scene_t scene_id)
	{
		auto& scene = _scenes[scene_id];

		if (scene.slots.empty())
			return false;

		for (auto& elem : scene.slots)
			if (elem.index != invalid)
			{
				// destroy
				auto& pool_end = _pool.back();
				auto  end_handle = pool_end.handle;
				auto& end_slot = _scenes[end_handle.scene].slots[end_handle.index];
				auto& destroyme = _pool[elem.index];
				end_slot.index = elem.index;

				// prepare for destruction
				std::swap(destroyme, pool_end);
				_pool.pop_back();
			}

		scene.slots.clear();
		return true;
	}
}