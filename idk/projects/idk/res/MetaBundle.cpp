#include "stdafx.h"
#include "MetaBundle.h"

namespace idk
{
	const SerializedMeta* MetaBundle::FetchMeta(string_view search_name) const
	{
		for (auto& elem : metadatas)
			if (elem.name == search_name)
				return &elem;

		return nullptr;
	}
	MetaBundle::operator bool() const
	{
		return metadatas.size();
	}
}