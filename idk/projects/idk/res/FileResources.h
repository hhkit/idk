#pragma once
#include <idk_config.h>
#include <meta/tag.h>
#include <res/ResourceMeta.h>
#include "FileResources_detail.h"

namespace idk
{
	struct SerializedResourceMeta;
	struct FileResources;

	class GenericRscHandle
	{
	public:
		template<typename Resource>
		GenericRscHandle(const RscHandle<Resource>&);

		template<typename Resource>
		RscHandle<Resource> As() const;
		template<typename Func> auto visit(Func&& func);
		template<typename Func> auto visit(Func&& func) const;

		explicit operator bool() const;
	private:
		detail::ResourceFile_helper<Resources>::GenericRscHandle _handle;
		friend class ResourceManager;
	};

	struct FileResources
	{
		vector<GenericRscHandle> resources;
	};

	struct GenericMetadata
	{
		Guid   guid;

		GenericMetadata() = default;
		template<typename T, 
			typename = sfinae<ResourceID<T> != ResourceCount && 
				!std::is_same_v<std::decay_t<T>, GenericMetadata>>
		>
		explicit GenericMetadata(const T&);
		explicit GenericMetadata(string_view serialized);

		template<typename T, typename = sfinae<has_tag_v<T, MetaTag>>> typename T::Metadata* GetMeta();
		template<typename T, typename = sfinae<has_tag_v<std::decay_t<T>, MetaTag>>> bool SetMeta(T&);
		explicit operator string() const;
	private:
		size_t resource_id = 0;
		shared_ptr<void> pimpl;
	};

	struct MetaFile
	{
		vector<GenericMetadata> resource_metas;
	};

	MetaFile save_meta(const FileResources&);

	template<typename T, typename>
	inline GenericMetadata::GenericMetadata(const T& resource)
		: guid{resource.GetHandle().guid},
		resource_id{ResourceID<T>},
		pimpl{}
	{
		if constexpr (has_tag_v<T, MetaTag>)
			pimpl = std::make_shared<typename T::Metadata>(resource.GetMeta());
	}
	template<typename T, typename>
	typename T::Metadata* GenericMetadata::GetMeta()
	{
		if (resource_id != ResourceID<T>)
			return nullptr;
		else
			return r_cast<typename T::Metadata*>(pimpl.get());
	}
	template<typename T, typename>
	inline bool GenericMetadata::SetMeta(T& obj)
	{
		if (ResourceID<T> != resource_id)
			return false;
		
		obj.SetMeta(GetMeta<T>());
		return true;
	}
}

#include "FileResources.inl"