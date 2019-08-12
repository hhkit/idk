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

		explicit operator bool() const;
		Res& operator*() const;
		Res* operator->() const;

		void Set(const Resource<Res>& g);
	};
}

#include "ResourceHandle.inl"