#include "pch.h"
#include "RegisterVulkanFactories.h"
#include <core/Core.h>
#include <file/FileSystem.h>
#include <vkn/VulkanMeshFactory.h>
#include <vkn/VulkanShaderModuleFactory.h>
#include <vkn/ShaderModule.h>
#include <vkn/VulkanGlslLoader.h>
#include <gfx/ShaderTemplateFactory.h>
#include <gfx/ShaderTemplateLoader.h>
#include <gfx/MaterialInstance.h>
#include <anim/SkeletonFactory.h>
#include <anim/Animation.h>
#include <vkn/VulkanTextureFactory.h>
#include <vkn/DDSLoader.h>
#include <vkn/VknRenderTargetFactory.h>
#include <vkn/VulkanPngLoader.h>
#include <vkn/VknFrameBufferFactory.h>
namespace idk::vkn
{

void RegisterFactories()
{
	Core::GetResourceManager().RegisterFactory<EasyFactory<MaterialInstance>>();
	Core::GetResourceManager().RegisterFactory<ShaderTemplateFactory>();
	Core::GetResourceManager().RegisterFactory<anim::SkeletonFactory>();
	Core::GetResourceManager().RegisterFactory<EasyFactory<anim::Animation>>();
	Core::GetResourceManager().RegisterFactory<MeshFactory>();
	Core::GetResourceManager().RegisterFactory<VulkanShaderModuleFactory>();
	Core::GetResourceManager().RegisterFactory<VulkanTextureFactory>();
	Core::GetResourceManager().RegisterFactory<VknRenderTargetFactory>();
	Core::GetResourceManager().RegisterFactory<VknFrameBufferFactory>();
	//Core::GetResourceManager().RegisterFactory<VulkanMaterialFactory>();
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Material>>(".frag");
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".fragspv");
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".vertspv");
	Core::GetResourceManager().RegisterLoader<PngLoader>(".png");
	Core::GetResourceManager().RegisterLoader<PngLoader>(".jpg");
	Core::GetResourceManager().RegisterLoader<PngLoader>(".jpeg");
	Core::GetResourceManager().RegisterLoader<VulkanGlslLoader>(".vert");
	Core::GetResourceManager().RegisterLoader<VulkanGlslLoader>(".frag");
	Core::GetResourceManager().RegisterLoader<VulkanSpvLoader>(".spv");
	Core::GetResourceManager().RegisterLoader<ShaderTemplateLoader>(".tmpt");
	Core::GetResourceManager().RegisterLoader<DdsLoader>(".dds");
}

}