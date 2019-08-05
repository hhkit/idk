#include "stdafx.h"
#include "GameObject.h"

namespace idk
{
	span<GenericHandle> GameObject::GetComponents()
	{
		return span<GenericHandle>(_components.begin()._Ptr, _components.end()._Ptr);
	}
}
