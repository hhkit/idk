#pragma once
#include <serialize/serialize.h>
namespace idk
{
	template<typename Meta>
	inline void MetaTag<Meta>::SetMeta(const Meta& inmeta)
	{
		OnMetaUpdate(inmeta);
		meta = inmeta;
		_dirtymeta = true;
	}
	template<typename Meta>
	inline const Meta& MetaTag<Meta>::GetMeta() const
	{
		return meta;
	}

	template<typename Res, typename>
	opt<typename Res::Metadata> SerializedMeta::GetMeta() const
	{
		if (t_hash == reflect::template typehash<Res>())
			return parse_text<typename Res::Metadata>(metadata);

		return std::nullopt;
	}
}