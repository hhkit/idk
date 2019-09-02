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
		Resource() = default;
		Resource(const Resource& rhs) : _loaded{ rhs._loaded.load() }, _must_load{ rhs._must_load.load() }, _keep_loaded{ rhs._keep_loaded.load() }, _dirty{ rhs._dirty } {}
		Resource& operator=(const Resource& rhs) { _dirty = rhs._dirty; return *this; }
		Resource(Resource&& rhs) : _loaded{ rhs._loaded.load() }, _must_load{ rhs._must_load.load() }, _keep_loaded{ rhs._keep_loaded.load() }, _dirty{ std::move(rhs._dirty) } {}
		Resource& operator=(Resource&& rhs) { std::swap(_dirty, rhs._dirty); return *this; }
		~Resource() = default;
	private:
		RscHandle<Res> _handle;
		atomic<bool>   _loaded      { true };
		atomic<bool>   _must_load   { false };
		atomic<bool>   _keep_loaded { false };
		bool           _dirty       { false };

		friend class ResourceManager;
		friend class ResourceFactory<Res>;
		template<typename T>
		friend struct detail::ResourceManager_detail;

		static_assert(ResourceID<Res> != ResourceCount, "Resource T must be included in the idk_config.h/Resources tuple");
	};
}