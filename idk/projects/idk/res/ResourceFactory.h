#pragma once
#include <idk.h>
#include <meta/tag.h>
#include <file/PathHandle.h>
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
		virtual unique_ptr<Resource> GenerateDefaultResource() = 0;
		virtual unique_ptr<Resource> Create() = 0;
		virtual ~ResourceFactory_impl() = default;
	};

	template<typename Res>
	class ResourceFactory_impl<Res, true>
	{
	public:
		using Resource = Res;
		virtual unique_ptr<Resource> GenerateDefaultResource() = 0;
		virtual unique_ptr<Resource> Create() = 0;
		virtual ~ResourceFactory_impl() = default;
	};

	template<typename Res>
	class ResourceFactory
		: public ResourceFactory_impl<Res, has_tag_v<Res, MetaTag>>
	{
	public:
		virtual void Init() {};
	};
}

//