#pragma once
#include <idk.h>
#include <meta/tag.h>
#include <res/ResourceMeta.h>

namespace idk
{
	template<typename Res>
	class ResourceFactory;

	template<typename Res, bool HasMeta>
	class ResourceFactory_impl;

	template<typename Res>
	class ResourceFactory_impl<Res, false>
	{
	public:
		using Resource = Res;
		virtual unique_ptr<Resource> Create() = 0;
		virtual unique_ptr<Resource> Create(string_view filepath) = 0;
		virtual ~ResourceFactory_impl() = default;
	};

	template<typename Res>
	class ResourceFactory_impl<Res, true>
	{
	public:
		using Resource = Res;
		virtual unique_ptr<Resource> Create() = 0;
		virtual unique_ptr<Resource> Create(string_view filepath) = 0;
		virtual unique_ptr<Resource> Create(string_view filepath, const typename Res::Metadata& m);
		virtual ~ResourceFactory_impl() = default;
	};

	template<typename Res>
	class ResourceFactory 
		: public ResourceFactory_impl<Res, has_tag_v<Res, MetaTag>>
	{
	};

	template<typename Res>
	unique_ptr<Res> ResourceFactory_impl<Res, true>::Create(string_view filepath, const typename Res::Metadata& m)
	{
		auto ptr = Create(filepath);
		ptr->SetMeta(m);
		return ptr;
	}
}

//