#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	template<typename Res>
	class Resource
	{
	public:
		RscHandle<Res> GetHandle() const { return handle_; }
	private:
		RscHandle<Res> handle_;
		atomic<bool>   loaded_    { false };
		bool           must_load_ { false };
		friend class ResourceManager;
	};
}