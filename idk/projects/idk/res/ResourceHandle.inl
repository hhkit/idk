#pragma once
#include <res/Resource.h>
#include <res/ResourceManager.h>

namespace idk
{
	template<typename Res>
	inline RscHandle<Res>::operator bool() const
	{
		return ResourceManager::instance->Validate(*this);
	}
	template<typename Res>
	inline Res& RscHandle<Res>::operator*() const
	{
		return Core::template GetSystem<ResourceManager>().Get(*this);
	}
	template<typename Res>
	inline Res* RscHandle<Res>::operator->() const
	{
		return &Core::template GetSystem<ResourceManager>().Get(*this);
	}
	template<typename Res>
	inline bool RscHandle<Res>::operator==(const RscHandle& rhs) const
	{
		return guid == rhs.guid;
	}
	template<typename Res>
	inline bool RscHandle<Res>::operator!=(const RscHandle& rhs) const
	{
		return guid != rhs.guid;
	}
	template<typename Res>
	inline void RscHandle<Res>::Set(const Resource<Res>& g)
	{
		*this = g.GetHandle();
	}
}