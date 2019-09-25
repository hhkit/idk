#pragma once
#include <idk.h>
#include <gfx/Material.h>
#include <res/EasyFactory.h>
namespace idk::vkn
{
	class VulkanMaterialFactory
		: public EasyFactory<Material>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource() override;
	};
}