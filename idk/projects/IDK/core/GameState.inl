#pragma once
#include <utility>
namespace idk
{
	template<typename T>
	span<T> GameState::GetObjectsOfType()
	{
		return detail::ObjectPools::GetPool<T>(_objects).GetSpan();
	}
	template<typename T>
	inline T* GameState::GetObject(const Handle<T>& handle)
	{
		return detail::ObjectPools::GetPool<T>(_objects).Get(handle);
	}
	template<typename T>
	inline Handle<T> GameState::CreateObject(uint8_t scene)
	{
		if constexpr (std::is_same_v<T, GameObject>)
		{
			auto hGameObject = detail::ObjectPools::GetPool<GameObject>(_objects).Create(scene);
			hGameObject->GameObject::template AddComponent<Transform>(); // today i learnt: fuck c++
			return hGameObject;
		}
		else
			return detail::ObjectPools::GetPool<T>(_objects).Create(scene);
	}
	template<typename T>
	inline Handle<T> GameState::CreateObject(const Handle<T>& handle)
	{
		if constexpr (std::is_same_v<T, GameObject>)
		{
			auto hGameObject = detail::ObjectPools::GetPool<T>(_objects).Create(handle);
			hGameObject->GameObject::template AddComponent<Transform>(); // today i learnt: fuck c++
			return hGameObject;
		}
		else
			return detail::ObjectPools::GetPool<T>(_objects).Create(handle);
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