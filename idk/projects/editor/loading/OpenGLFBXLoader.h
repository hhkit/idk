#pragma once
#include <idk.h>
#include <res/ExtensionLoader.h>

namespace idk
{
	// class aiScene;
	class OpenGLFBXLoader
		: public ExtensionLoader
	{
	public:
		FileResources Create(FileHandle path_to_resource) override;
		FileResources Create(FileHandle path_to_resource, span<SerializedResourceMeta> path_to_meta) override;

	private:
		struct Vertex
		{
			vec3 pos;
			vec3 normal;
			vec2 uv;
		};
	};
}