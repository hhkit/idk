#pragma once
#include <res/SaveableResourceLoader.h>
#include <res/FileLoader.h>
#include <vkn/VknRenderTarget.h>
namespace idk::vkn
{
	using VknRenderTargetLoader = EasySaveableResourceLoader<RenderTarget, VknRenderTarget>;

}