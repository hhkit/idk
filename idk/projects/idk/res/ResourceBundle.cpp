#include "stdafx.h"
#include "ResourceBundle.h"

namespace idk
{
	size_t ResourceBundle::Count() const
	{
		return handles.size();
	}
	span<const GenericResourceHandle> ResourceBundle::GetAll() const
	{
		return span<const GenericResourceHandle>(handles);
	}
}