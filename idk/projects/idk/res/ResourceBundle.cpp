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
	bool ResourceBundle::operator!=(const ResourceBundle& rhs) const
	{
		return false;
	}
	bool ResourceBundle::operator==(const ResourceBundle& rhs) const
	{
		auto lspan = GetAll();
		auto rspan = rhs.GetAll();
		
		if (lspan.size() != rspan.size())
			return false;

		for (auto& elem : lspan)
		{
			
		}

		return false;
	}
}