#pragma once
#include <utility>
#include "GameState.h"
#include <core/ObjectPool.inl>
#include <ds/span.inl>

namespace idk
{
	template<typename T>
	span<T> GameState::GetObjectsOfType()
	{
		if constexpr (std::is_const_v<T>)
		{
			const auto objspan = detail::ObjectPools::GetPool<std::decay_t<T>>(_objects).GetSpan();
			return span<T>{objspan.begin(), objspan.end()};
		}
		else
			return detail::ObjectPools::GetPool<std::decay_t<T>>(_objects).GetSpan();
	}
	template<typename T, typename Pd>
	inline unsigned GameState::SortObjectsOfType(Pd&& predicate)
	{
		return detail::ObjectPools::GetPool<T>(_objects).Defrag(std::forward<Pd>(predicate));
	}
	template<typename T>
	inline T* GameState::GetObject(const Handle<T>& handle)
	{
		return detail::ObjectPools::GetPool<T>(_objects).Get(handle);
	}
	template<typename T, typename ... Args>
	inline Handle<T> GameState::CreateObject(uint8_t scene, Args&& ... args)
	{
		auto new_handle = [&]()
		{
			if constexpr (std::is_same_v<T, GameObject>)
			{
				auto hGameObject = detail::ObjectPools::GetPool<GameObject>(_objects).Create(scene, std::forward<Args>(args)...);
				hGameObject->GameObject::template AddComponent<Transform>(); // today i learnt: fuck c++
				hGameObject->GameObject::template AddComponent<Name>();
				return hGameObject;
			}
			else
				return detail::ObjectPools::GetPool<T>(_objects).Create(scene, std::forward<Args>(args)...);
		}();
		_creation_queue.emplace_back(new_handle);
		return new_handle;
	}
#pragma optimize("",off)
	template<typename T, typename ... Args>
	inline Handle<T> GameState::CreateObject(const Handle<T>& handle, Args&& ... args)
	{
		auto new_handle = [&]()
		{
			if constexpr (std::is_same_v<T, GameObject>)
			{
				auto hGameObject = detail::ObjectPools::GetPool<T>(_objects).Create(handle, std::forward<Args>(args)...);
				hGameObject->GameObject::template AddComponent<Transform>(); // today i learnt: fuck c++
				hGameObject->GameObject::template AddComponent<Name>();
				return hGameObject;
			}
			else
				return detail::ObjectPools::GetPool<T>(_objects).Create(handle, std::forward<Args>(args)...);
		}();
		_creation_queue.emplace_back(new_handle);
		return new_handle;
	}
	template<typename T>
	inline bool GameState::ValidateHandle(const Handle<T>& handle)
	{
		return detail::ObjectPools::GetPool<T>(_objects).Validate(handle);
	}
	template<typename T>
	inline bool GameState::DestroyObjectNow(const Handle<T>& handle)
	{
		return detail::ObjectPools::GetPool<T>(_objects).Destroy(handle);
	}
}