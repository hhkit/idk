#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>

namespace idk
{
	template<typename Res>
	struct BaseResource;

	template<typename T>
	constexpr auto BaseResourceID = BaseResource<T>::value;

	template<typename Res>
	class Resource
	{
	public:
		using BaseResource = Res;

		RscHandle<Res> GetHandle() const { return _handle; }
		Resource() = default;

		string_view Name() const        { return _name; }
		void        Name(string_view n) { _name = n; }

	private:
		RscHandle<Res> _handle;
		string         _name;

		friend class ResourceManager;
		template<typename T>
		friend struct detail::ResourceManager_detail;

		static_assert(ResourceID<Res> != ResourceCount, "Resource T must be included in the idk_config.h/Resources tuple");
	};
}

#include "Resource.inl"