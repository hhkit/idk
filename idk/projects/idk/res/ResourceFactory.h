#pragma once
#include <idk.h>

namespace idk
{
	class ResourceMeta;

	template<typename Res>
	class ResourceFactory
	{
	public:
		using Resource = Res;
		virtual unique_ptr<Resource> Create() = 0;
		virtual unique_ptr<Resource> Create(string_view filepath, const ResourceMeta&) = 0;
		virtual ~ResourceFactory() = default;
	};
}