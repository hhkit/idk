#include "Scene.h"
#pragma once

namespace idk
{
	template<typename T>
	inline ObjectPool<T>& Scene::GetPool()
	{
		return *std::get<unique_ptr<ObjectPool<T>>>(pools);
	}
	
	template <typename T>
	bool Scene::CheckHandle(const ObjectHandle<T>& handle)
	{
		return GetPool<T>().validate(handle);
	}

	template<typename T>
	Scene::RetType<T> Scene::GetObject(const ObjectHandle<T>& handle)
	{
		return GetPool<T>().at(handle);
	}
	template<typename T>
	inline ObjectHandle<T> Scene::CreateObject()
	{
		return GetPool<T>().emplace();
	}
	template<typename T>
	inline ObjectHandle<T> Scene::CreateObjectAt(const ObjectHandle<T>& handle)
	{
		return GetPool<T>().emplace_at(handle);
	}
	template<typename T>
	inline bool Scene::DestroyObject(const ObjectHandle<T>& handle)
	{
		return GetPool<T>().remove(handle);
	}
}