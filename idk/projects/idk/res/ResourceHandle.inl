#pragma once
#include <res/Resource.h>
#include <res/ResourceManager.h>

namespace idk
{
	template<typename Res>
	inline RscHandle<Res>::operator bool() const
	{
		return Core::GetResourceManager().Validate(*this);
	}
	template<typename Res>
	inline Res& RscHandle<Res>::operator*() const
	{
		return Core::GetResourceManager().Get(*this);
	}
	template<typename Res>
	inline Res* RscHandle<Res>::operator->() const
	{
		return &Core::GetResourceManager().Get(*this);
	}
	template<typename Res>
	inline void RscHandle<Res>::Set(const Resource<Res>& g)
	{
		*this = g.GetHandle();
	}
}