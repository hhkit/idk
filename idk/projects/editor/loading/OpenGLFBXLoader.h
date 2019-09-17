#pragma once
#include <idk.h>
#include <res/ExtensionLoader.h>

namespace idk
{
	namespace fbx_loader_detail { struct Helper; }
	// class aiScene;
	class OpenGLFBXLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(PathHandle path_to_resource) override;
		FileResources Create(PathHandle path_to_resource, const MetaFile& path_to_meta) override;

		friend struct fbx_loader_detail::Helper;
	private:
		
		
	};
}