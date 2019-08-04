#include "Scene.h"
#pragma once

namespace idk
{
	template<typename T>
	inline ObjectPool<T>& Scene::GetPool()
	{
		return *std::get<unique_ptr<ObjectPool<T>>>(_pools);
	}
	
	template <typename T>
	bool Scene::Validate(const Handle<T>& handle)
	{
		return GetPool<T>().validate(handle);
	}

	template<typename T>
	Scene::RetType<T> Scene::GetObject(const Handle<T>& handle)
	{
		return GetPool<T>().at(handle);
	}
	template<typename T>
	Scene::RetType<T> Scene::GetObject(const GenericHandle& handle)
	{
		if (handle.type != Handle<T>::type_id)
			return {};
		return GetObject(handle_cast<T>(handle));
	}
	template<typename T, typename ... Args>
	inline Handle<T> Scene::CreateObject(Args&& ... args)
	{
		return GetPool<T>().emplace(std::forward<Args>(args)...);
	}
	template<typename T, typename ... Args>
	inline Handle<T> Scene::CreateObjectAt(const Handle<T>& handle, Args&& ... args)
	{
		return GetPool<T>().emplace_at(handle, std::forward<Args>(args)...);
	}
	template<typename T>
	inline bool Scene::DestroyObject(const Handle<T>& handle)
	{
		return GetPool<T>().remove(handle);
	}

	template<unsigned ...Indexes>
	inline auto Scene::GenValidateTable(std::index_sequence<Indexes...>)
	{
		return JumpTable<ValidateFn>
		{
			([](Scene& scene, const GenericHandle& handle)
				{
					return scene.Validate(handle_cast<std::decay_t<decltype(std::get<Indexes>(std::declval<Handleables>())) > > (handle));
				})...
		};
	}
	template<unsigned ...Indexes>
	inline auto Scene::GenCreateTable(std::index_sequence<Indexes...>)
	{
		return JumpTable<CreateFn>
		{
			([](Scene& scene) -> GenericHandle
				{
					if constexpr(Indexes == 0)
						return scene.CreateObject < std::decay_t<decltype(std::get<Indexes>(std::declval<Handleables>()))> > (&scene);
					else
						return scene.CreateObject < std::decay_t<decltype(std::get<Indexes>(std::declval<Handleables>()))> >();
				})...
		};
	}
	template<unsigned ...Indexes>
	inline auto Scene::GenCreateAtTable(std::index_sequence<Indexes...>)
	{
		return JumpTable<CreateAtFn>
		{
			([](Scene& scene, const GenericHandle& handle) -> GenericHandle
				{
					using type = std::decay_t<decltype(std::get<Indexes>(std::declval<Handleables>())) > ;
					if constexpr (std::is_same_v<GameObject, type>)
						return scene.CreateGameObjectAt(handle_cast<type> (handle));
					else
						return scene.CreateObjectAt(handle_cast<type > (handle));
				})...
		};
	}
	template<unsigned ...Indexes>
	inline auto Scene::GenDestroyTable(std::index_sequence<Indexes...>)
	{
		return JumpTable<DestroyFn>
		{
			([](Scene& scene, const GenericHandle& handle) 
				{
					return scene.DestroyObject(handle_cast<std::decay_t<decltype(std::get<Indexes>(std::declval<Handleables>())) > > (handle));
				})...
		};
	}
}