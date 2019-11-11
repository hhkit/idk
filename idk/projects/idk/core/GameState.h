#pragma once
#include <core/Component.h>
#include <scene/Scene.h>
#include "GameState_detail.h"
#undef GetObject

namespace idk::reflect
{
	class type;
}

namespace idk
{
	class Scene;
	class GameObject;

	class GameState
	{
	public:
		GameState();
		~GameState();

		bool       ActivateScene(uint8_t scene);
		bool       DeactivateScene(uint8_t scene);
		bool       DeactivateScene(Scene scene);
		bool       ValidateScene(const Scene& scene);

		bool CreateObject(const GenericHandle& handle);
		GenericHandle CreateComponent(const Handle<GameObject>&, reflect::type);
		GenericHandle CreateComponent(const Handle<GameObject>&, reflect::dynamic);
		GenericHandle CreateComponent(const Handle<GameObject>&, GenericHandle, reflect::dynamic);
		bool ValidateHandle(const GenericHandle& handle);
		void DestroyObject(const GenericHandle&);
		void FlushCreationQueue();
		void DestroyQueue();
		Handle<GameObject> GetGameObject(const GenericHandle& handle);

		uint8_t GetTypeID(const reflect::type&);

		template<typename T>  span<T>   GetObjectsOfType();
		template<typename T, typename Pd>  unsigned  SortObjectsOfType(Pd&&);

		template<typename T>  T*        GetObject(const Handle<T>& handle);
		template<typename T, typename ... Args>  Handle<T> CreateObject(uint8_t scene, Args&& ... args);
		template<typename T, typename ... Args>  Handle<T> CreateObject(const Handle<T>& handle, Args&& ... args);
		template<typename T>  bool      ValidateHandle(const Handle<T>& handle);
		template<typename T>  bool      DestroyObjectNow(const Handle<T>& handle);

		template<typename T> Signal<Handle<T>>& OnObjectCreate()  { return std::get<Signal<Handle<T>>>(_created_signals); }
		template<typename T> Signal<Handle<T>>& OnObjectDestroy() { return std::get<Signal<Handle<T>>>(_destroy_signals); }

		static GameState& GetGameState();
		static span<const char*> GetComponentNames();
	private:
		using TypeIDLUT       = hash_table<string_view, uint8_t>;

		static inline GameState* _instance = nullptr;
	
		detail::ObjectPools_t _objects;
		vector<GenericHandle> _creation_queue;
		vector<GenericHandle> _destruction_queue;

		detail::ObjectPools::SignalTuple _created_signals;
		detail::ObjectPools::SignalTuple _destroy_signals;

		static inline TypeIDLUT name_to_id_map{};

		friend detail::ObjectPools;
		template<typename T>
		friend struct detail::TableGenerator;

		template<typename T> bool QueuedForDestruction(Handle<T> obj) { return obj->_queued_for_destruction; }
		template<typename T> void QueueForDestruction(T& obj) { obj._queued_for_destruction = true; }
	};

	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(uint8_t);
	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(const Handle<class GameObject>&);
}

#include "GameState.inl"