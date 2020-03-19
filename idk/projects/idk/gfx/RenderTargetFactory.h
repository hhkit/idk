#pragma once
#include "RenderTarget.h"
#include <res/ResourceFactory.h>
namespace idk
{
	class RenderTargetFactory : public ResourceFactory<RenderTarget>
	{
	public:                       
		unique_ptr<RenderTarget> GenerateDefaultResource() override;	// generate default resource - the fallback resource if a handle fails
	private:
	};
}