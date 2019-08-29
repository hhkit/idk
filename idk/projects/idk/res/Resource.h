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
		atomic<bool>   _loaded      { true };
		atomic<bool>   _must_load   { false };
		atomic<bool>   _keep_loaded { false };
		atomic<bool>   _dirty       { true };

		friend class ResourceManager;
		friend class ResourceFactory<Res>;
		template<typename T>
		friend struct detail::ResourceManager_detail;
	};
}