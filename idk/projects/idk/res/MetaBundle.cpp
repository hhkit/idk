#include "stdafx.h"
#include <algorithm>
#include "MetaBundle.inl"

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
	bool MetaBundle::operator<(const MetaBundle& rhs) const
	{
		return std::lexicographical_compare(this->metadatas.begin(), this->metadatas.end(), rhs.metadatas.begin(), rhs.metadatas.end());
	}
}