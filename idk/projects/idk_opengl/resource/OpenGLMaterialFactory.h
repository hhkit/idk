#pragma once
#include <idk_opengl/resource/OpenGLMaterial.h>
#include <res/ResourceFactory.h>

namespace idk::ogl
{
	class OpenGLMaterialFactory
		: public ResourceFactory<Material>
	{
	public:
		unique_ptr<Material> Create() override;
		unique_ptr<Material> Create(string_view filepath) override;
		unique_ptr<Material> Create(string_view filepath, const ResourceMeta&) override;
	};
}