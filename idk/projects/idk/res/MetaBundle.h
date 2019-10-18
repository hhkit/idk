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

		template<typename T, typename = sfinae<has_tag_v<T, MetaTag>>>
		const SerializedMeta* FetchMeta() const;
	};
}

#include <serialize/text.h>
#include <reflect/reflect.h>
namespace idk
{
	template<typename T>
	void MetaBundle::Add(RscHandle<T> h)
	{
		if (h)
		{
			if constexpr (has_tag_v<T, MetaTag>)
				metadatas.emplace_back(SerializedMeta{ h.guid, string{h->Name()}, reflect::typehash<T>(), serialize_text(reflect::dynamic{ h->GetMeta() }) });
			else
				metadatas.emplace_back(SerializedMeta{ h.guid, string{h->Name()}, reflect::typehash<T>() });
		}
	}
	template<typename T, typename>
	const SerializedMeta* MetaBundle::FetchMeta() const
	{
		for (auto& elem : metadatas)
		{
			if (elem.t_hash == reflect::typehash<T>())
				return &elem;
		}
		return nullptr;
	}
}