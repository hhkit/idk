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

		static constexpr auto Count = std::tuple_size_v<Handleables>;

		// accessors
		template <typename T> ObjectPool<T>&  GetPool();
		template <typename T> bool            Validate(const ObjectHandle<T>& handle);

		// modifiers
		template <typename T> RetType<T>      GetObject(const ObjectHandle<T>& handle);
		template <typename T> RetType<T>      GetObject(const GenericHandle& handle);
		template <typename T> bool            DestroyObject(const ObjectHandle<T>& handle);
		template <typename T, typename ... Args> ObjectHandle<T> CreateObject(Args&& ...);
		template <typename T, typename ... Args> ObjectHandle<T> CreateObjectAt(const ObjectHandle<T>& handle, Args&& ...);

		// reflected variant
		bool            Validate(const GenericHandle& handle);
		GenericHandle   CreateObject(uint8_t type_id);
		GenericHandle   CreateObjectAt(const GenericHandle& handle);
		bool            DestroyObject(const GenericHandle& handle);

		// type specific
		ObjectHandle<GameObject> CreateGameObject();
		ObjectHandle<GameObject> CreateGameObjectAt(const ObjectHandle<GameObject>&);
	private:
		const uint8_t build_index;
		detail::ScenePool_t _pools;

		// jump tables
		template<typename Fn>
		using JumpTable  = std::array<Fn, Count>;
		using ValidateFn = bool(*)(Scene&, const GenericHandle&);
		using CreateFn   = GenericHandle(*)(Scene&);
		using CreateAtFn = GenericHandle(*)(Scene&, const GenericHandle&);
		using DestroyFn  = bool(*)(Scene&, const GenericHandle&);

		static inline bool jump_tables_initialized = false;
		static inline JumpTable<ValidateFn> validate_jt;
		static inline JumpTable<CreateFn>   create_jt;
		static inline JumpTable<CreateAtFn> createat_jt;
		static inline JumpTable<DestroyFn>  destroy_jt;

		template<unsigned ... Indexes> auto GenValidateTable(std::index_sequence<Indexes...>);
		template<unsigned ... Indexes> auto GenCreateTable  (std::index_sequence<Indexes...>);
		template<unsigned ... Indexes> auto GenCreateAtTable(std::index_sequence<Indexes...>);
		template<unsigned ... Indexes> auto GenDestroyTable (std::index_sequence<Indexes...>);
	};
};

#include "Scene.inl"