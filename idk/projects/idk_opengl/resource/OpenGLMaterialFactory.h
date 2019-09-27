#pragma once
#include <idk_opengl/resource/OpenGLMaterial.h>
#include <res/ResourceFactory.h>
#include <res/FileLoader.h>

namespace idk::ogl
{
	class OpenGLMaterialFactory
		: public ResourceFactory<Material>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource() override;
		unique_ptr<Material> Create() override;
	};

	class OpenGLMaterialLoader
		: public IFileLoader
	{
		ResourceBundle LoadFile(PathHandle filepath) override;
		ResourceBundle LoadFile(PathHandle filepath, const MetaBundle&) override;
	};
}