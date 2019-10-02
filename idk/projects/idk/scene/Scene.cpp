#include "stdafx.h"
#include "Scene.h"
#include <core/GameObject.h>
#include <util/ioutils.h>
#include "..\res\ResourceBundle.h"

namespace idk
{
	Scene::Scene(uint8_t scene_id_)
		: scene_id{ scene_id_ }
	{
	//	GameState::GetGameState().ActivateScene(scene_id_);
	}

	Scene::~Scene()
	{
	//	GameState::GetGameState().DeactivateScene(scene_id);
	}

	Handle<GameObject> Scene::CreateGameObject(const Handle<GameObject>& handle)
	{
		return GameState::GetGameState().CreateObject<GameObject>(handle);
	}

	Handle<GameObject> Scene::CreateGameObject()
	{
		return GameState::GetGameState().CreateObject<GameObject>(scene_id);
	}

	void Scene::DestroyGameObject(const Handle<GameObject>& go)
	{
		GameState::GetGameState().DestroyObject(go);
	}

	SceneLoadResult Scene::Load()
	{
		if (_loaded)
			return SceneLoadResult::Err_SceneAlreadyActive;

		auto path = Core::GetResourceManager().GetPath(GetHandle());

		if (!path)
			return SceneLoadResult::Err_ScenePathNotFound;

		GameState::GetGameState().ActivateScene(scene_id);
		auto stream = Core::GetSystem<FileSystem>().Open(*path, FS_PERMISSIONS::READ);
		parse_text(stringify(stream), *this);
		_loaded = true;
		return SceneLoadResult::Ok;
	}

	SceneUnloadResult Scene::Unload()
	{
		if (!_loaded)
			return SceneUnloadResult::Err_SceneAlreadyInactive;

		GameState::GetGameState().DeactivateScene(scene_id);
		_loaded = false;

		return SceneUnloadResult::Ok;
	}

	Scene::iterator Scene::begin() const
	{
		auto span = GameState::GetGameState().GetObjectsOfType<GameObject>();
		auto beg = span.begin();
		auto etr = span.end();

		while (beg != etr && beg->GetHandle().scene != scene_id)
			++beg; 

		return iterator{scene_id, beg, etr};
	}

	Scene::iterator Scene::end() const
	{
		auto span = GameState::GetGameState().GetObjectsOfType<GameObject>();
		return iterator{scene_id, span.end(), span.end() };
	}

	Scene GetScene(const GenericHandle& handle)
	{
		return Scene{ handle.scene };
	}

	GameObject& Scene::iterator::operator*()
	{
		return *curr_;
	}

	GameObject* Scene::iterator::operator->()
	{
		return curr_;
	}

	Scene::iterator& Scene::iterator::operator++()
	{
		do
		{
			++curr_;
		} while (curr_ != end_ && curr_->GetHandle().scene != scene_id_);

		return *this;
	}

	Scene::iterator  Scene::iterator::operator++(int)
	{
		auto copy = *this;
		operator++();
		return copy;
	}

	bool Scene::iterator::operator==(const iterator& rhs) const
	{
		return curr_ == rhs.curr_;
	}

	bool Scene::iterator::operator!=(const iterator& rhs) const
	{
		return curr_ != rhs.curr_;
	}

	Scene::iterator::iterator(uint8_t scene_id, GameObject* curr, GameObject* end)
		: curr_{curr}, end_{end}, scene_id_{scene_id}
	{
	}
}