#pragma once
#include <idk.h>
#include <res/FileLoader.h>

namespace idk
{
	namespace fbx_loader_detail { struct Helper; }
	// class aiScene;
	class OpenGLFBXLoader
		: public IFileLoader
	{
	public:
		ResourceBundle LoadFile(PathHandle path_to_resource) override;
		ResourceBundle LoadFile(PathHandle path_to_resource, const MetaBundle& path_to_meta) override;

		friend struct fbx_loader_detail::Helper;
	private:
		
		void updateBounds(const vec3& pos, vec3& minPos, vec3& maxPos) const;
	};
}