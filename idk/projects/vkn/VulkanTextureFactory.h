#pragma once
#include <idk.h>
#include <res/ResourceFactory.h>
#include <gfx/Texture.h>

namespace idk::vkn
{

	class VulkanTextureFactory
		: public ResourceFactory<Texture>
	{
	public:
		unique_ptr<Texture> GenerateDefaultResource() override;
		unique_ptr<Texture> Create() override;
		unique_ptr<Texture> Create(FileHandle filepath) override;
		unique_ptr<Texture> Create(FileHandle filepath, const Texture::Metadata&) override;
	};
}