#include "pch.h"
#include "RegisterVulkanFactories.h"
#include <file/FileSystem.h>
#include <vkn/VulkanMeshFactory.h>
#include <vkn/VulkanShaderModuleFactory.h>
#include <res/ForwardingExtensionLoader.h>
#include <vkn/VulkanGlslLoader.h>
#include <vkn/VulkanMaterialFactory.h>
#include <gfx/ShaderTemplateFactory.h>
#include <anim/SkeletonFactory.h>
#include <anim/AnimationFactory.h>
#include <vkn/VulkanTextureFactory.h>
#include <vkn/DDSLoader.h>
#include <vkn/VknFrameBufferFactory.h>
namespace idk::vkn
{

void RegisterFactories()
{
	Core::GetResourceManager().RegisterFactory<ShaderTemplateFactory>();
	Core::GetResourceManager().RegisterFactory<idk::anim::SkeletonFactory>();
	Core::GetResourceManager().RegisterFactory<idk::anim::AnimationFactory>();
	Core::GetResourceManager().RegisterFactory<MeshFactory>();
	Core::GetResourceManager().RegisterFactory<VulkanMaterialFactory>();
	Core::GetResourceManager().RegisterFactory<VulkanGlslLoader>();
	Core::GetResourceManager().RegisterFactory<VulkanTextureFactory>();
	Core::GetResourceManager().RegisterFactory<VknFrameBufferFactory>();
	//Core::GetResourceManager().RegisterFactory<VulkanMaterialFactory>();
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Material>>(".frag");
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".fragspv");
	//Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".vertspv");
	Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".vert");
	Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderProgram>>(".frag");
	Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<ShaderTemplate>>(".tmpt");
	Core::GetResourceManager().RegisterExtensionLoader<DdsLoader>(".dds");
}

}