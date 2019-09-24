#pragma once
#include <res/Guid.h>
#include <res/ResourceMeta.h>

namespace idk
{
	struct ResourceBundle;

	struct MetaBundle
	{
		vector<SerializedMeta> metadatas;

		template<typename T>
		void Add(RscHandle<T>);

		const SerializedMeta* FetchMeta(string_view name) const;
		// void AddBundle(const ResourceBundle&);
	};
}

#include <serialize/serialize.h>
#include <reflect/reflect.h>
namespace idk
{
	template<typename T>
	void MetaBundle::Add(RscHandle<T> h)
	{
		if (h)
		{
			if constexpr (has_tag_v<T, MetaTag>)
				metadatas.emplace_back(SerializedMeta{ h.guid, h->Name(), serialize_text(reflect::dynamic{ h->GetMeta() }) });
			else
				metadatas.emplace_back(SerializedMeta{ h.guid, h->Name() });
		}
	}
}