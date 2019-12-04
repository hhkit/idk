#pragma once
#include <file/PathHandle.h>

namespace idk
{
	class IAssetLoader
	{
	public:
		virtual void LoadAsset(PathHandle handle) = 0;
	};
}
