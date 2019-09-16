#include "pch.h"
#include "VulkanResourceManager.h"
#include <vkn/VulkanWin32GraphicsSystem.h>
namespace idk::vkn
{
VulkanResourceManager* idk::vkn::impl::GetRscManager()
{
	return &Core::GetSystem<VulkanWin32GraphicsSystem>().Instance().ResourceManager();
}


}