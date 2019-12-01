#include "stdafx.h"
#include <res/ResourceMeta.h>

namespace idk
{
	SerializedMeta::SerializedMeta(Guid guid, string_view name, string_view t_hash, string_view metadata)
		: guid{ guid }, name{name}, t_hash{t_hash}, metadata{metadata}
	{
	}
	bool SerializedMeta::operator<(const SerializedMeta& rhs) const noexcept
	{
#define COMP(FIELD) \
		if (this->FIELD < rhs.FIELD)\
			return true; \
		if (this->FIELD > rhs.FIELD) \
			return false;

		COMP(guid);
		COMP(name);
		COMP(t_hash);
		return this->metadata < rhs.metadata;
	}
}