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

	bool operator!=(const MetaBundle& lhs, const MetaBundle& rhs)
	{
		return !(lhs == rhs);
	}

	bool operator==(const MetaBundle& lhs, const MetaBundle& rhs)
	{
		return std::equal(lhs.metadatas.begin(), lhs.metadatas.end(), rhs.metadatas.begin());
	}
}