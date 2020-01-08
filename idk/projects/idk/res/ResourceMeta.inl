#pragma once
#include <serialize/text.h>
#include "res/ResourceMeta.h"
#include <reflect/reflect.inl>
#include <res/ResourceManager.inl>
namespace idk
{
	
	template<typename Res, typename Meta>
	inline Meta& MetaResource<Res, Meta>::GetMeta()
	{
		auto ptr = Core::GetResourceManager().GetControlBlock(this->GetHandle())->userdata.get();
		return *static_cast<Meta*>(ptr);
	}
	
	template<typename Res, typename Meta>
	inline void MetaResource<Res, Meta>::DirtyMeta()
	{
		Core::GetResourceManager().GetControlBlock(this->GetHandle())->dirty_meta = true;
	}

	template<typename Res, typename>
	opt<typename Res::Metadata> SerializedMeta::GetMeta() const
	{
        if (t_hash == reflect::template get_type<Res>().name())
        {
            auto res = parse_text<typename Res::Metadata>(metadata);
            if (res)
                return *res;
        }

		return std::nullopt;
	}
}