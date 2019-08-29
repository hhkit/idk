#pragma once

namespace idk
{
	template<typename Meta>
	inline void MetaTag<Meta>::SetMeta(const Meta& inmeta)
	{
		OnMetaUpdate(inmeta);
		meta = inmeta;
	}
	template<typename Meta>
	inline const Meta& MetaTag<Meta>::GetMeta() const
	{
		return meta;
	}
}