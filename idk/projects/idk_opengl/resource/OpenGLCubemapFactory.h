#pragma once
#include <idk_opengl/resource/OpenGLCubemap.h>
#include <res/ResourceFactory.h>

namespace idk::ogl
{
	class OpenGLCubeMapFactory
		: public ResourceFactory<CubeMap>
	{
	public:
		unique_ptr<CubeMap> GenerateDefaultResource() override;
		unique_ptr<CubeMap> Create() override;
		unique_ptr<CubeMap> Create(PathHandle filepath) override;
	};
}