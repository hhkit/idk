#include "stdafx.h"
#include "ResourceBundle.h"

namespace idk
{
	span<const GenericResourceHandle> ResourceBundle::GetAll() const
	{
		return span<const GenericResourceHandle>(handles);
	}
}