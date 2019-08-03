#include "stdafx.h"

#include <idk.h>
#include "Scene.h"
#include "Scene_detail.h"
#include "ComponentIncludes.h"

namespace idk
{
	Scene::Scene(uint8_t build_index)
		: build_index{ build_index }, pools { detail::ScenePool::Instantiate(build_index) }
	{
	}
	Scene::~Scene() = default;
	ObjectHandle<GameObject> Scene::CreateGameObject()
	{
		return CreateObject<GameObject>();
	}
}
