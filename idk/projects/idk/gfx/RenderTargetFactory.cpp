#include "stdafx.h"
#include "RenderTargetFactory.h"
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <ds/result.inl>

namespace idk
{
//#pragma optimize("",off)
	unique_ptr<RenderTarget> RenderTargetFactory::GenerateDefaultResource()
	{		   
 		PathHandle path_handle = "/engine_data/graphics/default.rtis";
		auto handle = Core::GetResourceManager().Load<RenderTarget>(path_handle);
		return {};
	}

}