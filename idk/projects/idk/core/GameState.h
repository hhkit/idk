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
		bool       ValidateScene(Scene& scene);

		bool CreateObject(const GenericHandle& handle);
		GenericHandle CreateComponent(const Handle<GameObject>&, reflect::type);
		GenericHandle CreateComponent(const Handle<GameObject>&, reflect::dynamic);
		bool ValidateHandle(const GenericHandle& handle);
		void DestroyObject(const GenericHandle&);
		void DestroyObject(const Handle<GameObject>&);
		void DestroyQueue();

		uint8_t GetTypeID(const reflect::type&);

		template<typename T>  span<T>   GetObjectsOfType();
		template<typename T, typename Pd>  unsigned  SortObjectsOfType(Pd&&);

		template<typename T>  T*        GetObject(const Handle<T>& handle);
		template<typename T, typename ... Args>  Handle<T> CreateObject(uint8_t scene, Args&& ... args);
		template<typename T, typename ... Args>  Handle<T> CreateObject(const Handle<T>& handle, Args&& ... args);
		template<typename T>  bool      ValidateHandle(const Handle<T>& handle);
		template<typename T>  bool      DestroyObjectNow(const Handle<T>& handle);

		static GameState& GetGameState();
		static span<const char*> GetComponentNames();
	private:
		template<typename Fn>
		using JumpTable = array<Fn, detail::ObjectPools::TypeCount>;

		using CreateTypeJT    = JumpTable<GenericHandle(*)(GameState&, const Handle<GameObject>&)>;
		using CreateDynamicJT = JumpTable<GenericHandle(*)(GameState&, const Handle<GameObject>&, const reflect::dynamic&)>;
		using CreateJT        = JumpTable<GenericHandle(*)(GameState&, const GenericHandle&)>;
		using DestroyJT       = JumpTable<void(*)(GameState&, const GenericHandle&)>;
		using QueueForDestructionJT = JumpTable<void(*)(GameState&, const GenericHandle&)>;
		using ValidateJT      = JumpTable<bool(*)(GameState&, const GenericHandle&)>;
		using TypeIDLUT       = hash_table<string_view, uint8_t>;

		static inline GameState* _instance = nullptr;
	
		detail::ObjectPools_t _objects;
		vector<GenericHandle> _destruction_queue;

		static inline CreateTypeJT    create_type_jt;
		static inline CreateDynamicJT create_dynamic_jt;
		static inline CreateJT        create_handles_jt;
		static inline DestroyJT       destroy_handles_jt;
		static inline ValidateJT      validate_handles_jt;
		static inline QueueForDestructionJT queue_for_destroy_jt;

		static inline TypeIDLUT name_to_id_map;

		friend detail::ObjectPools;
		template<typename T>
		friend struct detail::TableGenerator;

		template<typename T> void QueueForDestruction(T& obj) { obj._queued_for_destruction = true; }
	};

	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(uint8_t);
	extern template Handle<class GameObject> GameState::CreateObject<class GameObject>(const Handle<class GameObject>&);
}

#include "GameState.inl"