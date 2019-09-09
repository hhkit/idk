#pragma once
#include <idk_config.h>
#include <meta/tag.h>
#include <res/ResourceMeta.h>
#include <reflect/reflect.h>
#include "FileResources_detail.h"

namespace idk
{
	namespace reflect { class dynamic; }

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
		bool operator==(const GenericRscHandle&) const;
		bool operator!=(const GenericRscHandle&) const;
	private:
		detail::ResourceFile_helper<Resources>::GenericRscHandle _handle;
		friend class ResourceManager;
	};

	struct FileResources
	{
		vector<GenericRscHandle> resources;
		auto& operator[](size_t index)       { return resources[index]; }
		auto& operator[](size_t index) const { return resources[index]; }
	};

	struct MetaFile
	{
		vector<Guid>             guids;
		vector<reflect::dynamic> resource_metas;
	};

	MetaFile save_meta(const FileResources&);
}


#include "FileResources.inl"