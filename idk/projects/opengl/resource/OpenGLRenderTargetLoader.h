#pragma once
#include <res/SaveableResourceLoader.h>
#include <opengl/resource/FrameBuffer.h>

namespace idk::ogl
{
	using OpenGLRenderTargetLoader = EasySaveableResourceLoader<RenderTarget, OpenGLRenderTarget>;
}