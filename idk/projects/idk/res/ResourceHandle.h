#pragma once
#include <res/Guid.h>

namespace idk
{
	template<typename Res>
	class Resource;

	template<typename Res>
	struct RscHandle
	{
		Guid guid{};

		RscHandle() = default;
		RscHandle(const Guid&) : guid{ guid } {}

		void Set(const Resource<Res>& g);

		explicit operator bool() const;
		Res& operator*() const;
		Res* operator->() const;

		bool operator==(const RscHandle& rhs) const;
		bool operator!=(const RscHandle& rhs) const;
	};
}

#include "ResourceHandle.inl"