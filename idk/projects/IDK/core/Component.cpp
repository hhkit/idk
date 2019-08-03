#include "stdafx.h"
#include "Component.h"

namespace idk
{
	ObjectHandle<GameObject> GenericComponent::GetGameObject()
	{
		return _gameObject;
	}
}