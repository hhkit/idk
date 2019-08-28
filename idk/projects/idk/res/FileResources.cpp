#include "stdafx.h"
#include <IncludeResources.h>

namespace idk
{
	GenericRscHandle::operator bool() const
	{
		return std::visit([](auto handle) { return static_cast<bool>(handle); }, _handle);
	}

	vector<SerializedResourceMeta> serialize(const FileResources& resources)
	{
		auto retval = vector<SerializedResourceMeta>();

		for (auto& handle : resources.resources)
		{
			handle.visit([&](auto&& h)
			{
				SerializedResourceMeta m;
				m.guid = h.guid;
				if constexpr(has_tag_v<std::decay_t<decltype(h)>::Resource, MetaTag>)
					m.metadata = reflect::dynamic(h->GetMeta());
				retval.emplace_back(m);
			});
		}

		return retval;
	}
}
