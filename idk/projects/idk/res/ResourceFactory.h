#pragma once
#include <idk.h>
#include <meta/tag.h>
#include <file/PathHandle.h>
#include <res/ResourceMeta.h>

namespace idk
{
	template<typename Res>
	class ResourceFactory
	{
	public:
		using Resource = Res;

		virtual void Init() {};                                     // initialize factory, create default resources
		virtual unique_ptr<Resource> GenerateDefaultResource() = 0;	// generate default resource - the fallback resource if a handle fails
		virtual unique_ptr<Resource> Create() = 0;                  // generically create a resource
		virtual ~ResourceFactory_impl() = default;
	};
}

//