#include "stdafx.h"

#include <IncludeComponents.h>
#include <core/GameObject.h>
#include <reflect/reflect.h>

#include "GameState.h"

namespace idk::detail
{
	template<typename T>
	struct TableGenerator;

	template<typename ... Ts>
	struct TableGenerator<std::tuple<Ts...>>
	{
		using Tuple = std::tuple<Ts...>;

		template<typename T>
		using storage = shared_ptr<idk::ObjectPool<T>>;

		static constexpr auto TypeCount = sizeof...(Ts);
		using type = array<shared_ptr<void>, sizeof...(Ts)>;

		static auto Instantiate()
		{
			return type{
				std::allocate_shared< idk::ObjectPool<Ts>>(std::allocator<idk::ObjectPool<Ts>>{})...
			};
		}

		static bool ActivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[index_in_tuple_v<Ts, Tuple>].get())->ActivateScene(index)) && ...);
		}

		static bool DeactivateScene(type& pools, uint8_t index)
		{
			return ((s_cast<idk::ObjectPool<Ts>*>(pools[index_in_tuple_v<Ts, Tuple>].get())->DeactivateScene(index)) && ...);
		}

		static auto GenTypeLUT()
		{
			return GameState::TypeIDLUT{
				std::make_pair(reflect::get_type<Ts>().name(), index_in_tuple_v<Ts, Tuple>)...
			};
		}

		constexpr static auto GenCreateTypeJt()
		{
			return GameState::CreateTypeJT{
				[](GameState& gs, const Handle<GameObject>& go) -> GenericHandle
				{
					if constexpr (std::is_same_v<Ts, GameObject>)
					{
						(gs); (go);
						return GenericHandle{};
					}
					else
					{
						if constexpr (Ts::Unique)
						{
							auto component = go->GetComponent<Ts>();
							if (component)
								return component;
						}
						auto handle = gs.CreateObject<Ts>(go.scene);
						handle->_gameObject = go;
						go->_components.emplace_back(handle);
						return handle;
					}
				} ...
			};
		}

		constexpr static auto GenCreateDynamicJt()
		{
			return GameState::CreateDynamicJT{
				[](GameState& gs, const Handle<GameObject>& go, const reflect::dynamic& dyn) -> GenericHandle
				{
					if constexpr (std::is_same_v<Ts, GameObject>)
					{
						(gs); (go); (dyn);
						return GenericHandle{};
					}
					else
					{
						if constexpr (Ts::Unique)
						{
							auto component = go->GetComponent<Ts>();
							if (component)
								return component;
						}
						auto handle = gs.CreateObject<Ts>(go.scene, std::move(dyn.get<Ts>()));
						handle->_gameObject = go;
						go->_components.emplace_back(handle);
						return handle;
					}
				} ...
			};
		}

		constexpr static auto GenCreateJt()
		{
			return GameState::CreateJT{
				[](GameState& gs, const GenericHandle& handle) -> GenericHandle
				{
					return gs.CreateObject(handle_cast<Ts>(handle));
				} ...
			};
		}
		constexpr static auto GenValidateJt()
		{
			return GameState::ValidateJT{
				[](GameState& gs, const GenericHandle& handle) -> bool
				{
					return gs.ValidateHandle(handle_cast<Ts>(handle));
				} ...
			};
		}

		constexpr static auto GenQueueForDestructionJt()
		{
			return GameState::DestroyJT{
				[](GameState& gs, const GenericHandle& handle)
				{
					gs.QueueForDestruction(*handle_cast<Ts>(handle));
				} ...
			};
		}

		constexpr static auto GenDestructionJt()
		{
			return GameState::DestroyJT{
				[](GameState& gs, const GenericHandle& handle)
				{
					if constexpr (std::is_same_v<Ts, GameObject>)
						gs.DestroyObjectNow(handle_cast<Ts>(handle));
					else
					{
						if (const auto real_handle = handle_cast<Ts>(handle))
						{
							const auto entity = real_handle->GetGameObject();
							gs.DestroyObjectNow(real_handle);
							assert(entity);
							auto realent = std::addressof(*entity);
							auto itr = std::find(realent->_components.begin(), realent->_components.end(), handle);
							if (itr != realent->_components.end())
								entity->_components.erase(itr);
						}
					}
				} ...
			};
		}

		static auto GenComponentNameTable()
		{
			return std::array<const char*, 1 + ComponentCount>
			{
				[]() -> const char*
				{
					static string retval{ reflect::get_type<Ts>().name() };
					return retval.data();
				}()...
			};
		}

		constexpr static auto GenGetGameObjectTable()
		{
			return std::array<Handle<GameObject>(*)(const GenericHandle&), 1 + ComponentCount>
			{
				[](const GenericHandle& handle) -> Handle<GameObject>
				{
					if constexpr (std::is_same_v<Ts, GameObject>)
						return handle_cast<GameObject>(handle);
					else
					{
						if (const auto real_handle = handle_cast<Ts>(handle))
							return real_handle->GetGameObject();
						else
							return {};
					}
				} ...
			};
		}
	};

	using TableGen = TableGenerator<Handleables>;
}

namespace idk
{
	template Handle<class GameObject> GameState::CreateObject<class GameObject>(uint8_t);
	template Handle<class GameObject> GameState::CreateObject<class GameObject>(const Handle<class GameObject>&);

	GameState::GameState()
		: _objects{detail::TableGen::Instantiate()}
	{
		assert(_instance == nullptr);
		name_to_id_map       = detail::TableGen::GenTypeLUT();
		_instance = this;
	}

	GameState::~GameState()
	{
		assert(_instance);
		_instance = nullptr;
	}

	bool GameState::ActivateScene(uint8_t scene)
	{
		return detail::TableGen::ActivateScene(_objects, scene);
	}
	bool GameState::DeactivateScene(uint8_t scene)
	{
		return detail::TableGen::DeactivateScene(_objects, scene);
	}
	bool GameState::DeactivateScene(Scene scene)
	{
		return DeactivateScene(scene.scene_id);
	}
	bool GameState::ValidateScene(const Scene& scene)
	{
		return static_cast<ObjectPool<GameObject>*>(std::get<0>(_objects).get())->ValidateScene(scene.scene_id);
	}
	bool GameState::CreateObject(const GenericHandle& handle)
	{
		constexpr auto create_handles_jt = detail::TableGen::GenCreateJt();
		return s_cast<bool>(create_handles_jt[handle.type](*this, handle));
	}
	GenericHandle GameState::CreateComponent(const Handle<GameObject>& handle, reflect::type type)
	{
		const auto id = GetTypeID(type);
		constexpr auto create_type_jt = detail::TableGen::GenCreateTypeJt();
		return id <= ComponentCount ? create_type_jt[id](*this, handle) : GenericHandle{};
	}
	GenericHandle GameState::CreateComponent(const Handle<GameObject>& handle, reflect::dynamic dyn)
	{
		const auto id = GetTypeID(dyn.type);
		constexpr auto create_dynamic_jt = detail::TableGen::GenCreateDynamicJt();
		return id <= ComponentCount ? create_dynamic_jt[GetTypeID(dyn.type)](*this, handle, dyn) : GenericHandle{};
	}
	bool GameState::ValidateHandle(const GenericHandle& handle)
	{
		constexpr auto validate_handles_jt = detail::TableGen::GenValidateJt();
		return validate_handles_jt[handle.type](*this, handle);
	}
	void GameState::DestroyObject(const GenericHandle& handle)
	{
		constexpr auto queue_for_destroy_jt = detail::TableGen::GenQueueForDestructionJt();
		if (handle)
		{
			queue_for_destroy_jt[handle.type](*this, handle);
			_destruction_queue.emplace_back(handle);
		}
	}
	void GameState::DestroyObject(const Handle<GameObject>& handle)
	{
		if (handle)
		{
			for (auto& elem : handle->GetComponents())
				DestroyObject(elem);
			handle->_queued_for_destruction = true;
			_destruction_queue.emplace_back(handle);
		}

	}
	void GameState::DestroyQueue()
	{
		constexpr auto destroy_handles_jt = detail::TableGen::GenDestructionJt();
		for (auto& elem : _destruction_queue)
			destroy_handles_jt[elem.type](*this, elem);
		_destruction_queue.clear();
	}
	Handle<GameObject> GameState::GetGameObject(const GenericHandle& handle)
	{
		constexpr auto get_go_jt = detail::TableGen::GenGetGameObjectTable();
		return get_go_jt[handle.type](handle);
	}
	uint8_t GameState::GetTypeID(const reflect::type& type)
	{
		auto itr = type.valid() ? name_to_id_map.find(type.name()) : name_to_id_map.end();
		return itr != name_to_id_map.end() ? itr->second : std::numeric_limits<uint8_t>::max();
	}
	GameState& GameState::GetGameState()
	{
		return *_instance;
	}

	span<const char*> GameState::GetComponentNames()
	{
		static auto arr = detail::TableGen::GenComponentNameTable();
		return span<const char*>(&arr[1], std::data( arr) + std::size(arr));
	}
}
