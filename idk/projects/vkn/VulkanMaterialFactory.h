#pragma once
#include <idk.h>
#include <gfx/Material.h>
#include <vkn/VulkanMaterial.h>
#include <res/EasyFactory.h>
namespace idk::vkn
{
	class VulkanMaterialFactory
		: public EasyFactory<VulkanMaterial>
	{
	public:
		unique_ptr<Material> GenerateDefaultResource() override;
	};
}