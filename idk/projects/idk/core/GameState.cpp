#include "stdafx.h"
#include <core/GameObject.h>
#include <core/IncludeComponents.h>
#include "GameState.h"
#include <reflect/reflect.h>

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

		static auto GenCreateTypeJt()
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
						auto handle = gs.CreateObject<Ts>(go.scene);
						handle->_gameObject = go;
						return handle;
					}
				} ...
			};
		}

		static auto GenCreateJt()
		{
			return GameState::CreateJT{
				[](GameState& gs, const GenericHandle& handle) -> GenericHandle
				{
					return gs.CreateObject(handle_cast<Ts>(handle));
				} ...
			};
		}
		static auto GenValidateJt()
		{
			return GameState::ValidateJT{
				[](GameState& gs, const GenericHandle& handle) -> bool
				{
					return gs.ValidateHandle(handle_cast<Ts>(handle));
				} ...
			};
		}

		static auto GenDestructionJt()
		{
			return GameState::DestroyJT{
				[](GameState& gs, const GenericHandle& handle)
				{
					if constexpr (std::is_same_v<Ts, GameObject>)
						gs.DestroyObjectNow(handle_cast<Ts>(handle));
					else
					{
						auto real_handle = handle_cast<Ts>(handle);
						if (real_handle)
						{
							auto entity = real_handle->GetGameObject();
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
		name_to_type_id     = detail::TableGen::GenTypeLUT();
		create_type_jt      = detail::TableGen::GenCreateTypeJt();
		create_handles_jt   = detail::TableGen::GenCreateJt();
		destroy_handles_jt  = detail::TableGen::GenDestructionJt();
		validate_handles_jt = detail::TableGen::GenValidateJt();
		_instance = this;
	}

	GameState::~GameState()
	{
		assert(_instance);
		_instance = nullptr;
	}

	opt<Scene> GameState::ActivateScene(uint8_t scene)
	{
		if (detail::TableGen::ActivateScene(_objects, scene))
			return Scene{ scene };
		else
			return {};
	}
	bool GameState::DectivateScene(uint8_t scene)
	{
		return detail::TableGen::DeactivateScene(_objects, scene);
	}
	bool GameState::DectivateScene(Scene scene)
	{
		return DectivateScene(scene.scene_id);
	}
	bool GameState::CreateObject(const GenericHandle& handle)
	{
		return create_handles_jt[handle.type](*this, handle);
	}
	GenericHandle GameState::CreateComponent(const Handle<GameObject>& handle, reflect::type type)
	{
		return create_type_jt[GetTypeID(type)](*this, handle);
	}
	bool GameState::ValidateHandle(const GenericHandle& handle)
	{
		return validate_handles_jt[handle.type](*this, handle);
	}
	void GameState::DestroyObject(const GenericHandle& handle)
	{
		if (handle)
			_destruction_queue.emplace_back(handle);
	}
	void GameState::DestroyObject(const Handle<GameObject>& handle)
	{
		if (handle)
		{
			for (auto& elem : handle->GetComponents())
				DestroyObject(elem);
			_destruction_queue.emplace_back(handle);
		}

	}
	void GameState::DestroyQueue()
	{
		for (auto& elem : _destruction_queue)
			destroy_handles_jt[elem.type](*this, elem);
		_destruction_queue.clear();
	}
	uint8_t GameState::GetTypeID(const reflect::type& type)
	{
		auto itr = name_to_type_id.find(type.name());
		return itr != name_to_type_id.end() ? itr->second : std::numeric_limits<uint8_t>::max();
	}
	GameState& GameState::GetGameState()
	{
		return *_instance;
	}
}
