#pragma once
#include <res/Guid.h>

namespace idk
{
	template<typename Res>
	struct BaseResource;

	template<typename Res>
	using BaseResource_t = typename BaseResource<Res>::type;

	template<typename Res>
	class Resource;

	template<typename Res>
	struct RscHandle
	{
		using Resource = Res;

		Guid guid{};

		RscHandle() = default;
		template<typename Other, typename = std::enable_if_t<std::is_base_of_v<Other, Res>>>
		explicit RscHandle(RscHandle<Other> other) : guid{ other.guid } {};

		RscHandle(const Guid& guid) : guid{ guid } {}

		void Set(const idk::Resource<Res>& g);
		template<typename T>
		T& as() const;

		explicit operator bool() const;
		template<typename Other, typename = std::enable_if_t<std::is_base_of_v<Other, Res>>>
		explicit operator RscHandle<Other>() const;

		Res& operator*() const;
		Res* operator->() const;

		bool operator==(const RscHandle& rhs) const;
		bool operator!=(const RscHandle& rhs) const;
	};
}

// hashtable support
namespace std
{
	template<typename Res>
	struct hash <idk::RscHandle<Res>>
	{
		size_t operator()(const idk::RscHandle<Res>& res) const noexcept
		{
			return std::hash<idk::Guid>()(res.guid);
		}
	};
}

#include "ResourceHandle.inl"