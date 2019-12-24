#include "stdafx.h"
#include "ResourceBundle.h"
#include <ds/span.inl>

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
		return !operator==(rhs);
	}
	bool ResourceBundle::operator==(const ResourceBundle& rhs) const
	{
		auto lspan = GetAll();
		auto rspan = rhs.GetAll();
		
		if (lspan.size() != rspan.size())
			return false;

		for (auto& elem : lspan)
		{
			auto itr = std::find(rhs.handles.begin(), rhs.handles.end(), elem);
			if (itr == rhs.handles.end())
				return false;
		}

		return true;
	}
}