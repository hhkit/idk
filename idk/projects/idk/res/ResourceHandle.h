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
		RscHandle(const Guid& guid) : guid{ guid } {}

		void Set(const Resource<Res>& g);

		explicit operator bool() const;
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
	struct hash <idk::Resource<Res>>
	{
		size_t operator()(const idk::Resource<Res>& res) const noexcept
		{
			const idk::u64* p = reinterpret_cast<const idk::u64*>(&res.guid);
			std::hash<idk::u64> hash;
			return hash(p[0]) ^ hash(p[1]);
		}
	};
}

#include "ResourceHandle.inl"