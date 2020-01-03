#pragma once
#include "MetaBundle.h"
#include <serialize/text.inl> //serialize_text
#include <reflect/reflect.inl> //get_type
#include <res/ResourceManager.inl>
#include <res/ResourceMeta.inl> //GetMeta
namespace idk
{
	template<typename T>
	void MetaBundle::Add(RscHandle<T> h)
	{
		if (h)
		{
			if constexpr (has_tag_v<T, MetaResource>)
				metadatas.emplace_back(SerializedMeta{ h.guid, string{h->Name()}, string{reflect::get_type<T>().name()}, serialize_text(reflect::dynamic{ h->GetMeta() }) });
			else
				metadatas.emplace_back(SerializedMeta{ h.guid, string{h->Name()}, string{reflect::get_type<T>().name()} });
		}
	}
	template<typename T>
	const SerializedMeta* MetaBundle::FetchMeta() const
	{
		for (auto& elem : metadatas)
		{
			if (elem.t_hash == reflect::get_type<T>().name())
				return &elem;
		}
		return nullptr;
	}

	template<typename T>
	const SerializedMeta* MetaBundle::FetchMeta(string_view name) const
	{
		for (auto& elem : metadatas)
		{
			if (elem.t_hash == reflect::get_type<T>().name() && elem.name == name)
				return &elem;
		}
		return nullptr;
	}

	template<typename FullResType>
	inline RscHandle<FullResType> MetaBundle::CreateResource() const
	{
		auto m = FetchMeta<typename FullResType::BaseResource>();
		return m ? ResourceManager::Instance().template LoaderEmplaceResource<FullResType>(m->guid) : ResourceManager::Instance().template LoaderEmplaceResource<FullResType>();
	}

	template<typename FullResType>
	inline RscHandle<FullResType> MetaBundle::CreateResource(string_view name) const
	{
		auto m = FetchMeta<typename FullResType::BaseResource>(name);
		return m ? ResourceManager::Instance().template LoaderEmplaceResource<FullResType>(m->guid) : ResourceManager::Instance().template LoaderEmplaceResource<FullResType>();
	}
}