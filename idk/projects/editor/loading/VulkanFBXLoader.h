#pragma once
#include <idk.h>
#include <res/ExtensionLoader.h>

namespace idk
{
	namespace fbx_loader_detail { struct Helper; }
	// class aiScene;
	class VulkanFBXLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(FileHandle path_to_resource) override;
		FileResources Create(FileHandle path_to_resource, const MetaFile& path_to_meta) override;

		friend struct fbx_loader_detail::Helper;
		VulkanFBXLoader();
		~VulkanFBXLoader();
	private:
		struct Data;

		unique_ptr<Data> _data;
	};
}