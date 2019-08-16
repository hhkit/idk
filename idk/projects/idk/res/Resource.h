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
		atomic<bool>   _loaded    { false };
		bool           _must_load { false };
		friend class ResourceManager;
	};
}