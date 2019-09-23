#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	template<typename T>
	constexpr auto BaseResourceID = detail::BaseResource<T>::value;

	template<typename Res>
	class Resource
	{
	public:
		using BaseResource = Res;

		RscHandle<Res> GetHandle() const { return _handle; }
		Resource() = default;
	private:
		RscHandle<Res> _handle;

		friend class ResourceManager;
		friend class ResourceFactory<Res>;
		template<typename T>
		friend struct detail::ResourceManager_detail;

		static_assert(ResourceID<Res> != ResourceCount, "Resource T must be included in the idk_config.h/Resources tuple");
	};
}

#include "Resource.inl"