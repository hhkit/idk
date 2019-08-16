#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	template<typename Res>
	class Resource
	{
	public:
		RscHandle<Res> GetHandle() const { return _handle; }
	private:
		RscHandle<Res> _handle;
		atomic<bool>   _loaded      { false };
		atomic<bool>   _must_load   { false };
		atomic<bool>   _keep_loaded { false };
		friend class ResourceManager;
	};
}