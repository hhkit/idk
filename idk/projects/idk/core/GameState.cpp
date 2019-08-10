#include "stdafx.h"
#include <core/GameObject.h>
#include <core/IncludeComponents.h>
#include "GameState.h"

namespace idk
{
	template Handle<class GameObject> GameState::CreateObject<class GameObject>(uint8_t);
	template Handle<class GameObject> GameState::CreateObject<class GameObject>(const Handle<class GameObject>&);

	GameState::GameState()
		: _objects{detail::ObjectPools::Instantiate()}
	{
		assert(_instance == nullptr);
		destroy_handles_jt = detail::ObjectPools::GenDestructionJt();
		validate_handles_jt = detail::ObjectPools::GenValidateJt();
		_instance = this;
	}

	GameState::~GameState()
	{
		assert(_instance);
		_instance = nullptr;
	}

	opt<Scene> GameState::ActivateScene(uint8_t scene)
	{
		if (detail::ObjectPools::ActivateScene(_objects, scene))
			return Scene{ scene };
		else
			return {};
	}
	bool GameState::DectivateScene(uint8_t scene)
	{
		return detail::ObjectPools::DeactivateScene(_objects, scene);
	}
	bool GameState::DectivateScene(Scene scene)
	{
		return DectivateScene(scene.scene_id);
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
	GameState& GameState::GetGameState()
	{
		return *_instance;
	}
}
