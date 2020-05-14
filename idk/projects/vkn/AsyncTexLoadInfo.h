#pragma once

#include <vkn/VknTextureLoader.h>
namespace idk::vkn
{
	struct AsyncTexLoadInfo
	{
		shared_ptr<void> context_data;
		InputTexInfo  iti;
		TexCreateInfo tci;
		TextureOptions to;
	};
}