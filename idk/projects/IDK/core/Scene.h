#pragma once

#include <idk.h>
#include <core/Handleables.h>
#include <core/ObjectPool.h>

#include "Scene_detail.h"

namespace idk
{
	class Scene
	{
	public:
		template<typename T> 
		using RetType = T *;

		Scene(uint8_t build_index = 0);
		~Scene();

		// accessors
		template <typename T> ObjectPool<T>&  GetPool();
		template <typename T> bool            CheckHandle(const ObjectHandle<T>& handle);

		// modifiers
		template <typename T> RetType<T>      GetObject(const ObjectHandle<T>& handle);
		template <typename T> ObjectHandle<T> CreateObject();
		template <typename T> ObjectHandle<T> CreateObjectAt(const ObjectHandle<T>& handle);
		template <typename T> bool            DestroyObject(const ObjectHandle<T>& handle);

		// reflected variant

		// type specific
		ObjectHandle<GameObject> CreateGameObject();
	private:
		const uint8_t build_index;
		detail::ScenePool_t pools;
	};
};
#include "Scene.inl"