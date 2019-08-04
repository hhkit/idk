#include "stdafx.h"

#include <idk.h>
#include "Scene.h"
#include "Scene_detail.h"
#include "ComponentIncludes.h"

namespace idk
{
	Scene::Scene(uint8_t build_index)
		: build_index{ build_index }, _pools { detail::ScenePool::Instantiate(build_index) }
	{
		if (jump_tables_initialized == false)
		{
			validate_jt = GenValidateTable(std::make_index_sequence<Count>{});
			create_jt = GenCreateTable(std::make_index_sequence<Count>{});
			createat_jt = GenCreateAtTable(std::make_index_sequence<Count>{});
			destroy_jt = GenDestroyTable(std::make_index_sequence<Count>{});
			jump_tables_initialized = true;
		}
	}

	Scene::~Scene() = default;
	bool Scene::Validate(const GenericHandle& handle)
	{
		return validate_jt[handle.type](*this, handle);
	}

	GenericHandle Scene::CreateObject(uint8_t type_id)
	{
		return create_jt[type_id](*this);
	}

	GenericHandle Scene::CreateObjectAt(const GenericHandle& handle)
	{
		return createat_jt[handle.type](*this, handle);
	}

	bool Scene::DestroyObject(const GenericHandle& handle)
	{
		return destroy_jt[handle.type](*this, handle);
	}

	Handle<GameObject> Scene::CreateGameObject()
	{
		return CreateObject<GameObject>(this);
	}
	Handle<GameObject> Scene::CreateGameObjectAt(const Handle<GameObject>& handle)
	{
		return CreateObjectAt(handle, this);
	}
}
