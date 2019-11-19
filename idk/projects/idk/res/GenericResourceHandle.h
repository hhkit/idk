#pragma once
#include <idk.h>
#include <res/ResourceHandle.h>
#include <meta/variant.h>
namespace idk
{
	struct GenericResourceHandle
		: variant_wrap_t<tuple_to_variant_t<Resources>, RscHandle>
	{
	private:
		using Base = variant_wrap_t<tuple_to_variant_t<Resources>, RscHandle>;
	public:
		using Base::Base;
		using Base::operator=;

		template<typename T, typename = sfinae<index_in_tuple_v<T, Resources> == ResourceCount>> // inherited handles
		GenericResourceHandle(RscHandle<T> handle);
		GenericResourceHandle(Guid guid, string typehash);

		Guid   guid() const;
		size_t resource_id() const;

		template<typename T> RscHandle<T>& AsHandle();
		template<typename T> const RscHandle<T>& AsHandle() const;

		template<typename T, typename = sfinae<index_in_tuple_v<T, Resources> == ResourceCount>> // inherited handles
		GenericResourceHandle& operator=(RscHandle<T> handle);
	};

	template<typename T, typename>
	GenericResourceHandle::GenericResourceHandle(RscHandle<T> handle)
		: Base{ RscHandle<typename T::BaseResource>{handle} }
	{
	}

	template<typename T, typename>
	inline GenericResourceHandle& GenericResourceHandle::operator=(RscHandle<T> handle)
	{
		Base::operator=(RscHandle<typename T::BaseResource>{handle});
		return *this;
	}

	template<typename T>
	RscHandle<T>& GenericResourceHandle::AsHandle()
	{
		return reinterpret_cast<RscHandle<T>&>(std::get<BaseResourceID<T>>(*this));
	}

	template<typename T>
	const RscHandle<T>& GenericResourceHandle::AsHandle() const
	{
		return reinterpret_cast<const RscHandle<T>&>(std::get<BaseResourceID<T>>(*this));
	}
}

namespace std
{
	template<>
	struct hash<idk::GenericResourceHandle>
	{
		size_t operator()(const idk::GenericResourceHandle& handle) const
		{
			return std::hash<idk::Guid>{}(handle.guid()) << 6 ^ std::hash<size_t>{}(handle.resource_id()) >> 2;
		}
	};
}