#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"

namespace idk
{
	Scene::Scene(uint8_t scene_id)
		: scene_id{ scene_id }
	{
	}

	Handle<GameObject> Scene::CreateGameObject()
	{
		return GameState::GetGameState().CreateObject<GameObject>(scene_id);
	}

	void Scene::DestroyGameObject(const Handle<GameObject>& go)
	{
		GameState::GetGameState().DestroyObject(go);
	}

	span<GameObject> Scene::GetAllGameObjects()
	{
		return GameState::GetGameState().GetObjectsOfType<GameObject>();
	}

	Scene GetScene(const GenericHandle& handle)
	{
		return Scene{ handle.scene };
	}
}